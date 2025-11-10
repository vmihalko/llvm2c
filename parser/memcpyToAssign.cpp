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

                            // 1. Try to get the actual pointer types (with or without bitcast)
                            const llvm::Value* dst = stripCast(dstVal);
                            const llvm::Value* src = stripCast(srcVal);
                            
                            // If stripCast didn't find a bitcast, use the original values
                            if (!dst) dst = dstVal;
                            if (!src) src = srcVal;

                            // 2. Get the types
                            auto* dstTy = dst->getType();
                            auto* srcTy = src->getType();

                            // check if both are pointers to the same type
                            if (dstTy != srcTy || !dstTy->isPointerTy()) {
                                // stop processing this call
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

                                    // Handle RefExpr (e.g., &var)
                                    Expr* srcVar = nullptr;
                                    Expr* dstVar = nullptr;
                                    
                                    if (auto* srcRef = llvm::dyn_cast_or_null<RefExpr>(srcExpr)) {
                                        srcVar = srcRef->expr;
                                    } else if (auto* srcVal = llvm::dyn_cast_or_null<Value>(srcExpr)) {
                                        // Direct Value (variable name)
                                        srcVar = srcVal;
                                    }
                                    
                                    if (auto* dstRef = llvm::dyn_cast_or_null<RefExpr>(dstExpr)) {
                                        dstVar = dstRef->expr;
                                    } else if (auto* dstVal = llvm::dyn_cast_or_null<Value>(dstExpr)) {
                                        // Direct Value (variable name)
                                        dstVar = dstVal;
                                    }
                                    
                                    if (srcVar && dstVar) {
                                        auto assignment = std::make_unique<AssignExpr>(dstVar, srcVar);
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

    program.addPass(PassType::MemcpyToAssignment);
}
