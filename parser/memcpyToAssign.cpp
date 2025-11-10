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
                    std::string baseFuncName = funcName;

                    // Handle both llvm.memcpy and memcpy
                    if (funcName.substr(0,4) == "llvm") {
                        baseFuncName = trimPrefix(funcName);
                    }
                    
                    if (baseFuncName == "memcpy") {
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

                                // Handle different expression types to extract the actual variable
                                // Unwrap casts to get to the underlying expression
                                auto unwrapExpr = [](Expr* expr) -> Expr* {
                                    while (auto* cast = llvm::dyn_cast_or_null<CastExpr>(expr)) {
                                        expr = cast->expr;
                                    }
                                    return expr;
                                };
                                
                                Expr* srcUnwrapped = unwrapExpr(srcExpr);
                                Expr* dstUnwrapped = unwrapExpr(dstExpr);
                                
                                // Handle different expression types
                                // For memcpy(&dst, &src, size), we want to generate *dst = *src or dst = src
                                // depending on whether dst/src are pointers or structs
                                Expr* dstForAssign = nullptr;
                                Expr* srcForAssign = nullptr;
                                
                                // Handle destination: if it's RefExpr, check the type to determine if we need to dereference
                                if (auto* dstRef = llvm::dyn_cast_or_null<RefExpr>(dstUnwrapped)) {
                                    // Destination is &var
                                    // Check if var is a pointer type - if so, we need *var, not *(&var)
                                    auto* innerType = dstRef->expr->getType();
                                    if (auto* ptrType = llvm::dyn_cast_or_null<PointerType>(innerType)) {
                                        // var is a pointer, so &var is pointer-to-pointer
                                        // We want *var (dereference the pointer), not *(&var) (which is just var)
                                        auto deref = std::make_unique<DerefExpr>(dstRef->expr);
                                        dstForAssign = deref.get();
                                        myBlock->addOwnership(std::move(deref));
                                    } else {
                                        // var is not a pointer (it's a struct), so &var is pointer-to-struct
                                        // We want *(&var) which is var
                                        auto deref = std::make_unique<DerefExpr>(dstRef->expr);
                                        dstForAssign = deref.get();
                                        myBlock->addOwnership(std::move(deref));
                                    }
                                } else if (auto* dstDeref = llvm::dyn_cast_or_null<DerefExpr>(dstUnwrapped)) {
                                    // Destination is already *ptr, use as is
                                    dstForAssign = dstUnwrapped;
                                } else {
                                    // Destination is a Value or other expression, use as is
                                    dstForAssign = dstUnwrapped;
                                }
                                
                                // Handle source: if it's RefExpr, dereference it
                                // But if it's AggregateElement or other non-pointer expression, use directly
                                if (auto* srcRef = llvm::dyn_cast_or_null<RefExpr>(srcUnwrapped)) {
                                    // Check if the inner expression is a pointer type
                                    // If it's a Value that's a pointer, we need to dereference
                                    // If it's an AggregateElement (struct member), we use it directly
                                    if (auto* innerVal = llvm::dyn_cast_or_null<Value>(srcRef->expr)) {
                                        // Source is &var where var is a pointer, dereference: *(&var)
                                        auto deref = std::make_unique<DerefExpr>(srcRef->expr);
                                        srcForAssign = deref.get();
                                        myBlock->addOwnership(std::move(deref));
                                    } else {
                                        // Source is &(struct.member), use the member directly (no dereference)
                                        srcForAssign = srcRef->expr;
                                    }
                                } else if (auto* srcDeref = llvm::dyn_cast_or_null<DerefExpr>(srcUnwrapped)) {
                                    // Source is already *ptr, use as is
                                    srcForAssign = srcUnwrapped;
                                } else {
                                    // Source is a Value, AggregateElement, or other expression, use as is
                                    srcForAssign = srcUnwrapped;
                                }
                                
                                if (dstForAssign && srcForAssign) {
                                    auto assignment = std::make_unique<AssignExpr>(dstForAssign, srcForAssign);
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

    program.addPass(PassType::MemcpyToAssignment);
}
