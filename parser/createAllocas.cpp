#include <llvm/IR/Instruction.h>
#include "../core/Program.h"
#include "../core/Func.h"
#include "../core/Block.h"

#include "constval.h"

void createAllocas(const llvm::Module* module, Program& program) {
    assert(program.isPassCompleted(PassType::CreateBlocks));

    for (const auto& function : module->functions()) {
        auto* func = program.getFunction(&function);
        for (const auto& block : function) {
            auto* myBlock = func->getBlock(&block);

            for (const auto& ins : block) {
                if (ins.getOpcode() == llvm::Instruction::Alloca) {
                    std::unique_ptr<Value> theVariable;
                    std::unique_ptr<StackAlloc> alloc;
                    const auto *allocaInst = llvm::cast<const llvm::AllocaInst>(&ins);
                    if (allocaInst->isArrayAllocation()) {
                        const llvm::Value* llsizeVal = allocaInst->getArraySize();
                        Expr *sizeExpr = program.getExpr(llsizeVal);
                        if (!sizeExpr) {
                            // see if the operand is just the alloca of MAX itself
                            if (auto *load = llvm::dyn_cast<llvm::LoadInst>(llsizeVal)) {
                                sizeExpr = program.getExpr(load->getPointerOperand()); // this is MAX
                            }
                        }
                        if (!sizeExpr)
                            sizeExpr = createConstantValue(llsizeVal, program); // fallback

                        if (auto ref = llvm::dyn_cast_or_null<RefExpr>(sizeExpr)) {
                            // For VLAs, we want the variable name, not the address
                            // Create a stable Value expression for the VLA size
                            if (auto innerVal = llvm::dyn_cast_or_null<Value>(ref->expr)) {
                                // Create a new Value expression that will be owned by the program
                                auto stableSize = std::make_unique<Value>(innerVal->valueName, innerVal->getType());
                                sizeExpr = stableSize.get();
                                program.addOwnership(std::move(stableSize));
                            }
                        }

                        Type* elemTy = func->getType(allocaInst->getAllocatedType());
                        Type *vlaTy = program.typeHandler.variableLengthArrayOf(elemTy, sizeExpr);

                        theVariable = std::make_unique<Value>(func->getVarName(), vlaTy);
                        alloc       = std::make_unique<StackAlloc>(theVariable.get());
                        
                        // For VLAs, just add ownership (no expression yet - will be inserted by InsertVLADecls pass)
                        if (!llvm::isa<llvm::ConstantInt>(allocaInst->getArraySize())) {
                            myBlock->addOwnership(std::move(alloc));
                        } else {
                            // Fixed-size array, add as expression immediately
                            myBlock->addExprAndOwnership(std::move(alloc));
                        }

                        // The createExpr for &ins is performed unconditionally below; no need to duplicate here
                    } else  {
                        // normal alloca on the stack
                        theVariable = std::make_unique<Value>(func->getVarName(), func->getType(allocaInst->getAllocatedType()));
                        alloc = std::make_unique<StackAlloc>(theVariable.get());
                        myBlock->addExprAndOwnership(std::move(alloc));
                    }
                    func->createExpr(&ins, std::make_unique<RefExpr>(theVariable.get(), program.typeHandler.pointerTo(theVariable.get()->getType())));
                    myBlock->addOwnership(std::move(theVariable));
                }
            }

        }
    }

    program.addPass(PassType::CreateAllocas);
}

// Honour the LLVM `byval` parameter attribute.
//
// `byval` means the pointer argument is passed BY VALUE: the callee logically
// receives a private copy of the pointee, and stores through the parameter must
// NOT be visible to the caller.  llvm2c otherwise renders the parameter as a
// plain pointer and passes the caller's address straight through, so callee
// writes corrupt the caller's object (a real, UB-independent miscompilation).
//
// Fix (callee-side entry snapshot -- structurally identical to an alloca that
// is initialised from the incoming pointer):
//   * keep the C signature parameter as the incoming pointer (`struct S* p`);
//     the caller and every prototype stay unchanged, so the call site keeps
//     passing the caller-owned address -- correct, because the copy is made
//     inside the callee at the call boundary;
//   * at function entry declare a local of the pointee type and snapshot the
//     pointee into it (`copy = *p;`);
//   * redirect every body use of the parameter to the address of that local
//     (`&copy`), so all GEP/store/load machinery -- which already treats the
//     parameter as a pointer base -- now operates on the private copy.
//
// The snapshot is taken before any body statement, matching langref's "a hidden
// copy of the pointee is made between the caller and the callee": later writes
// through an aliasing pointer do not perturb the already-captured copy.
//
// Declaration-only functions have no body to copy into and are skipped (the
// definition, and thus the copy, lives in its own translation unit; N/A for the
// whole-program csmith inputs this targets).
void createByvalCopies(const llvm::Module* module, Program& program) {
    assert(program.isPassCompleted(PassType::CreateAllocas));

    for (const auto& function : module->functions()) {
        if (function.isDeclaration())
            continue;

        auto* func = program.getFunction(&function);
        if (!func)
            continue;

        Block* entry = func->getBlock(&function.getEntryBlock());
        if (!entry)
            continue;

        // Built here, inserted at the FRONT of the entry block afterwards so the
        // snapshot runs before every body statement (body exprs are appended
        // later by createExpressions).
        std::vector<Expr*> prologue;

        for (const llvm::Argument& arg : function.args()) {
            if (!arg.hasByValAttr())
                continue;

            // The original parameter Expr (a pointer Value).  MUST be captured
            // *before* the map is redirected below: building DerefExpr against
            // the post-remap expr would yield `*(&copy)` == `copy`, i.e. the
            // no-op self-assignment `copy = copy` with copy left uninitialised.
            Expr* paramExpr = func->getExpr(&arg);
            if (!paramExpr)
                continue;

            llvm::Type* pointeeTy = arg.getParamByValType();
            if (!pointeeTy)
                continue;
            Type* copyTy = func->getType(pointeeTy);

            // Local holding the callee-private copy.
            auto copyVarUp = std::make_unique<Value>(func->getVarName(), copyTy);
            Value* copyVar = copyVarUp.get();

            // `<pointee-type> copy;`
            auto declUp = std::make_unique<StackAlloc>(copyVar);
            // `copy = *param;`  (an array pointee becomes memmove via ExprWriter)
            auto derefUp = std::make_unique<DerefExpr>(paramExpr);
            auto assignUp = std::make_unique<AssignExpr>(copyVar, derefUp.get());

            prologue.push_back(declUp.get());
            prologue.push_back(assignUp.get());

            program.addOwnership(std::move(copyVarUp));
            program.addOwnership(std::move(declUp));
            program.addOwnership(std::move(derefUp));
            program.addOwnership(std::move(assignUp));

            // Redirect all body uses of the parameter to `&copy`.  Done AFTER
            // DerefExpr(paramExpr) is constructed above.
            func->createExpr(&arg, std::make_unique<RefExpr>(copyVar, program.typeHandler.pointerTo(copyTy)));
        }

        if (!prologue.empty()) {
            entry->expressions.insert(entry->expressions.begin(), prologue.begin(), prologue.end());
        }
    }
}
