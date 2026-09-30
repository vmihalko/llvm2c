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
                    if (allocaInst->isArrayAllocation() && !llvm::isa<llvm::ConstantInt>(allocaInst->getArraySize())) {
                        // VLA.  Its size is usually an expression over values
                        // createExpressions has not built yet (`int a[n + 1]`),
                        // so the real type and the declaration are made there,
                        // at the alloca's position.  Placeholder until then.
                        Type* elemTy = func->getType(allocaInst->getAllocatedType());
                        theVariable = std::make_unique<Value>(func->getVarName(), program.typeHandler.pointerTo(elemTy));
                        alloc       = std::make_unique<StackAlloc>(theVariable.get());
                        program.vlaAllocs[allocaInst] = alloc.get();
                        myBlock->addOwnership(std::move(alloc));
                    } else if (allocaInst->isArrayAllocation()) {
                        // constant count: unchanged from before
                        const llvm::Value* llsizeVal = allocaInst->getArraySize();
                        Expr *sizeExpr = program.getExpr(llsizeVal);
                        if (!sizeExpr)
                            sizeExpr = createConstantValue(llsizeVal, program);
                        Type* elemTy = func->getType(allocaInst->getAllocatedType());
                        theVariable = std::make_unique<Value>(func->getVarName(), program.typeHandler.variableLengthArrayOf(elemTy, sizeExpr));
                        alloc       = std::make_unique<StackAlloc>(theVariable.get());
                        myBlock->addExprAndOwnership(std::move(alloc));
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

// A VLA alloca is an `elem*` in the IR, so its address is `&a[0]`, not `&a`
// (an `elem(*)[n]`: pointer arithmetic would stride whole arrays and a deref
// would yield an array, i.e. an array assignment gcc/CBMC reject).
// createAllocas maps the alloca to `&a` because InsertVLADecls and the
// metadata passes look through that RefExpr to the variable; rewrite it in
// place once they are done and before refDeref folds `*(&a)` to `a`.
void fixVLAAddresses(const llvm::Module* module, Program& program) {
    for (const auto& function : module->functions()) {
        auto* func = program.getFunction(&function);
        if (!func)
            continue;
        for (const auto& block : function) {
            for (const auto& ins : block) {
                const auto* AI = llvm::dyn_cast<llvm::AllocaInst>(&ins);
                if (!AI || !AI->isArrayAllocation() || llvm::isa<llvm::ConstantInt>(AI->getArraySize()))
                    continue;
                auto* ref = llvm::dyn_cast_or_null<RefExpr>(func->getExpr(AI));
                if (!ref)
                    continue;
                auto* var = llvm::dyn_cast_or_null<Value>(ref->expr);
                auto* arrTy = var ? llvm::dyn_cast_or_null<ArrayType>(var->getType()) : nullptr;
                if (!arrTy || !arrTy->dynSize)
                    continue;
                Expr* zero = program.makeExpr<Value>("0", program.typeHandler.slong.get());
                ref->expr = program.makeExpr<ArrayElement>(var, zero, arrTy->type);
                ref->setType(program.typeHandler.pointerTo(arrTy->type));
            }
        }
    }
}
