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

                        auto ref = std::make_unique<RefExpr>(theVariable.get(),
                                program.typeHandler.pointerTo(elemTy));
                        func->createExpr(&ins,std::move(ref));
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
