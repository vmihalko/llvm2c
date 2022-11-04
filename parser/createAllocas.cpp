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

                    // from https://llvm.org/doxygen/Local_8cpp_source.html#l01600
                    // Determine whether this alloca is either a VariableLengthArray or an array.
                    // static bool isArray(AllocaInst *AI) {
                    //     return AI->isArrayAllocation() ||
                    //             (AI->getAllocatedType() && AI->getAllocatedType()->isArrayTy());
                    if ((allocaInst->getAllocatedType()->isArrayTy() ||
                                   (allocaInst->getAllocatedType() && allocaInst->isArrayAllocation()))) {
                                    // continue;
                       //llvm::errs() << *allocaInst << "\n";
                       //llvm::errs() << *allocaInst->getAllocatedType() << "\n";
                        auto *llsize = allocaInst->getArraySize();
                        Expr *size = nullptr;
                        if (llvm::isa<llvm::ConstantInt>(llsize)) {
                            size = createConstantValue(llsize, program);
                        } else {
                            size = program.getExpr(llsize);
                        }
                        if (!size) {
                            llvm::errs() << "Unhandled size argument of alloca: " << *allocaInst << "\n";
                            throw std::invalid_argument("Unhandled alloca");
                        }

                       //theVariable = std::make_unique<Value>(func->getVarName(), func->getType(allocaInst->getType()));
                       //alloc = std::make_unique<StackAlloc>(theVariable.get());
                        // std::vector<Expr*> params;
                        // params.push_back(size);
                        // auto allocacall = std::make_unique<CallExpr>(
                        //         nullptr,
                        //         "alloca",
                        //         params,
                        //         func->getType(allocaInst->getType()));

                        // assign the result of the call to the variable
                        theVariable = std::make_unique<Value>(func->getVarName(), func->getType(allocaInst->getAllocatedType()));
                        alloc = std::make_unique<StackAlloc>(theVariable.get());
                        // auto assign = std::make_unique<AssignExpr>(theVariable.get(), allocacall.get());
                        myBlock->addExprAndOwnership(std::move(alloc));
                        // myBlock->addOwnership(std::move(allocacall));
                        // myBlock->addExprAndOwnership(std::move(assign));
                        //func->createExpr(&ins, std::move(allocacall));
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
