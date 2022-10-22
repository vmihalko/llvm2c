#include "cfunc.h"
#include "toinst.h"

#include "../core/Program.h"
#include "../core/Func.h"
#include "../core/Block.h"

#include <llvm/IR/Instruction.h>

#include <algorithm>

const llvm::Value* stripCast(const llvm::Value* val) {

    if (auto* BC = llvm::dyn_cast_or_null<llvm::BitCastInst>(val)) {
        return BC->getOperand(0);
    }

    if (auto* CE = const_cast<llvm::ConstantExpr*>(llvm::dyn_cast_or_null<llvm::ConstantExpr>(val))) {
        auto inst = toInst(CE);
        return stripCast(inst.get());
    }

    return nullptr;
}

void memcpyToAssignment(const llvm::Module* module, Program& program) {
    assert(program.isPassCompleted(PassType::CreateExpressions));

    for (const auto& function : module->functions()) {
        auto* func = program.getFunction(&function);
        for (const auto& block : function) {
            auto* myBlock = func->getBlock(&block);

            for (const auto& ins : block) {
                if (auto callInst = llvm::dyn_cast_or_null<llvm::CallInst>(&ins)) {
                    if (!callInst->getCalledFunction()) {
                        continue;
                    }

                    std::string funcName = callInst->getCalledFunction()->getName().str();

                    if (funcName.substr(0,4) == "llvm") {
                        funcName = trimPrefix(funcName);
                        if (funcName == "memcpy") {
                            auto* dstVal = callInst->getArgOperand(0);
                            auto* srcVal = callInst->getArgOperand(1);
                            auto* size = callInst->getArgOperand(2);

                            // 1. check if operands are bitcasted pointer to structs
                            if (auto* dst = stripCast(dstVal)) {
                                if (auto* src = stripCast(srcVal)) {

                                    // 2. strip the bitcast 
                                    auto* dstTy = dst->getType();
                                    auto* srcTy = src->getType();

                                    // check if both structs share the same type
                                    if (dstTy != srcTy) {
                                        // stop processing this call
                                        continue;
                                    }

                                    if (!dstTy->isPointerTy()) {
                                        continue;
                                    }

                                    auto *PT = dstTy->getPointerElementType();

                                    // 3. check if the memcpy size is a constant
                                    if (auto* constSize = llvm::dyn_cast_or_null<llvm::ConstantInt>(size)) {
                                        size_t expectedSize = module->getDataLayout().getTypeAllocSize(PT);

                                        // 4. check if the memcpy size is size of the whole type
                                        if (expectedSize == constSize->getValue().getLimitedValue()) {
                                            auto* exprToReplace = func->getExpr(callInst);
                                            auto* srcExpr = func->getExpr(src);
                                            auto* dstExpr = func->getExpr(dst);

                                            if (auto* srcRef = llvm::dyn_cast_or_null<RefExpr>(srcExpr)) {
                                                if (auto* dstRef = llvm::dyn_cast_or_null<RefExpr>(dstExpr)) {
                                                    GlobalValue *ptrGV = nullptr;
                                                    if (auto rgv = program.getGlobalVar(src))
                                                        ptrGV = llvm::dyn_cast_or_null<GlobalValue>(rgv->expr);

                                                    auto assignment = std::make_unique<AssignExpr>(dstRef->expr, (ptrGV && ptrGV->isPrivate ? ptrGV->value : srcRef->expr));
                                                    Expr* newExpr = assignment.get();
                                                    myBlock->addOwnership(std::move(assignment));
                                                    std::replace(myBlock->expressions.begin(), myBlock->expressions.end(), exprToReplace, newExpr);
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }

        }
    }

    program.addPass(PassType::MemcpyToAssignment);
}
