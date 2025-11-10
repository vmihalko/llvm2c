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
                                // Use the original values (dstVal, srcVal) to get expressions, not the stripped ones
                                // This ensures we get the correct expression structure including any casts
                                auto* srcExpr = func->getExpr(srcVal);
                                auto* dstExpr = func->getExpr(dstVal);

                                // Handle different expression types to extract the actual variable
                                // Unwrap casts and nested RefExpr/DerefExpr to get to the underlying expression
                                auto unwrapExpr = [](Expr* expr) -> Expr* {
                                    while (true) {
                                        if (auto* cast = llvm::dyn_cast_or_null<CastExpr>(expr)) {
                                            expr = cast->expr;
                                        } else if (auto* ref = llvm::dyn_cast_or_null<RefExpr>(expr)) {
                                            // Unwrap &(&expr) -> &expr, but keep the outer RefExpr
                                            if (auto* innerRef = llvm::dyn_cast_or_null<RefExpr>(ref->expr)) {
                                                expr = innerRef;
                                            } else {
                                                break;
                                            }
                                        } else {
                                            break;
                                        }
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
                                
                                // Helper to get the actual variable from a RefExpr, unwrapping nested RefExpr and DerefExpr
                                auto getInnerExpr = [](RefExpr* ref) -> Expr* {
                                    Expr* inner = ref->expr;
                                    // Unwrap nested RefExpr: &(&var) -> &var -> var
                                    // Also unwrap DerefExpr: &(*var) -> var
                                    while (true) {
                                        if (auto* innerRef = llvm::dyn_cast_or_null<RefExpr>(inner)) {
                                            inner = innerRef->expr;
                                        } else if (auto* innerDeref = llvm::dyn_cast_or_null<DerefExpr>(inner)) {
                                            inner = innerDeref->expr;
                                        } else {
                                            break;
                                        }
                                    }
                                    return inner;
                                };
                                
                                // Handle destination: if it's RefExpr, check the type to determine if we need to dereference
                                if (auto* dstRef = llvm::dyn_cast_or_null<RefExpr>(dstUnwrapped)) {
                                    // Destination is &var (possibly nested)
                                    Expr* innerExpr = getInnerExpr(dstRef);
                                    // Ensure innerExpr is not a RefExpr or DerefExpr (should be a Value or AggregateElement)
                                    while (auto* innerRef = llvm::dyn_cast_or_null<RefExpr>(innerExpr)) {
                                        innerExpr = getInnerExpr(innerRef);
                                    }
                                    while (auto* innerDeref = llvm::dyn_cast_or_null<DerefExpr>(innerExpr)) {
                                        innerExpr = innerDeref->expr;
                                    }
                                    
                                    // Check the type of &var (dstRef) to see if it's a pointer-to-pointer or pointer-to-struct
                                    // If &var is pointer-to-pointer, then var is a pointer, and we want *var (dereference)
                                    // If &var is pointer-to-struct, then var is a struct, and we want *(&var) = var (dereference)
                                    auto* refType = dstRef->getType();  // Type of &var
                                    if (auto* refPtrType = llvm::dyn_cast_or_null<PointerType>(refType)) {
                                        // &var is a pointer, check what it points to
                                        auto* pointedType = refPtrType->type;  // What &var points to
                                        if (auto* pointedPtrType = llvm::dyn_cast_or_null<PointerType>(pointedType)) {
                                            // &var points to a pointer, so var is a pointer
                                            // For memcpy(&ptr, &other_ptr, size), we want *ptr = *other_ptr (dereference both)
                                            // So we need to dereference var: *var
                                            auto deref = std::make_unique<DerefExpr>(innerExpr);
                                            dstForAssign = deref.get();
                                            myBlock->addOwnership(std::move(deref));
                                        } else {
                                            // &var points to a struct, so var is a struct
                                            // For memcpy(&struct, &other_struct, size), we want struct = other_struct
                                            // So we need *(&var) which is var
                                            auto deref = std::make_unique<DerefExpr>(innerExpr);
                                            dstForAssign = deref.get();
                                            myBlock->addOwnership(std::move(deref));
                                        }
                                    } else {
                                        // Fallback: check innerExpr's type
                                        auto* innerType = innerExpr->getType();
                                        if (auto* ptrType = llvm::dyn_cast_or_null<PointerType>(innerType)) {
                                            // var is a pointer - dereference it to get the struct value
                                            auto deref = std::make_unique<DerefExpr>(innerExpr);
                                            dstForAssign = deref.get();
                                            myBlock->addOwnership(std::move(deref));
                                        } else {
                                            // var is a struct - dereference to get the struct value
                                            auto deref = std::make_unique<DerefExpr>(innerExpr);
                                            dstForAssign = deref.get();
                                            myBlock->addOwnership(std::move(deref));
                                        }
                                    }
                                } else if (auto* dstDeref = llvm::dyn_cast_or_null<DerefExpr>(dstUnwrapped)) {
                                    // Destination is already *ptr, use as is
                                    dstForAssign = dstUnwrapped;
                                } else {
                                    // Destination is a Value or other expression
                                    // Check if it's a pointer type
                                    // For memcpy(ptr, other_ptr, size), we want *ptr = *other_ptr (dereference both)
                                    // For memcpy(ptr, &struct, size), we want *ptr = struct (dereference dst)
                                    auto* dstType = dstUnwrapped->getType();
                                    if (auto* ptrType = llvm::dyn_cast_or_null<PointerType>(dstType)) {
                                        // dst is a pointer - dereference it to get the struct value
                                        auto deref = std::make_unique<DerefExpr>(dstUnwrapped);
                                        dstForAssign = deref.get();
                                        myBlock->addOwnership(std::move(deref));
                                    } else {
                                        // dst is not a pointer, use as is
                                        dstForAssign = dstUnwrapped;
                                    }
                                }
                                
                                // Handle source: if it's RefExpr, check what's inside
                                if (auto* srcRef = llvm::dyn_cast_or_null<RefExpr>(srcUnwrapped)) {
                                    // Source is &something (possibly nested)
                                    Expr* innerExpr = getInnerExpr(srcRef);
                                    // Ensure innerExpr is not a RefExpr or DerefExpr
                                    while (auto* innerRef = llvm::dyn_cast_or_null<RefExpr>(innerExpr)) {
                                        innerExpr = getInnerExpr(innerRef);
                                    }
                                    while (auto* innerDeref = llvm::dyn_cast_or_null<DerefExpr>(innerExpr)) {
                                        innerExpr = innerDeref->expr;
                                    }
                                    
                                    // Check the type of &var (srcRef) to see if it's a pointer-to-pointer or pointer-to-struct
                                    auto* refType = srcRef->getType();  // Type of &var
                                    if (auto* refPtrType = llvm::dyn_cast_or_null<PointerType>(refType)) {
                                        // &var is a pointer, check what it points to
                                        auto* pointedType = refPtrType->type;  // What &var points to
                                        if (auto* pointedPtrType = llvm::dyn_cast_or_null<PointerType>(pointedType)) {
                                            // &var points to a pointer, so var is a pointer
                                            // For memcpy(&ptr, &other_ptr, size), we want *ptr = *other_ptr (dereference both)
                                            // So we need to dereference var: *var
                                            auto deref = std::make_unique<DerefExpr>(innerExpr);
                                            srcForAssign = deref.get();
                                            myBlock->addOwnership(std::move(deref));
                                        } else {
                                            // &var points to a struct, so var is a struct
                                            // For memcpy(&ptr, &struct, size), we want *ptr = struct
                                            // So we use struct directly (no dereference needed)
                                            srcForAssign = innerExpr;
                                        }
                                    } else {
                                        // Fallback: check innerExpr's type
                                        auto* innerType = innerExpr->getType();
                                        if (auto* ptrType = llvm::dyn_cast_or_null<PointerType>(innerType)) {
                                            // var is a pointer - dereference it to get the struct value
                                            auto deref = std::make_unique<DerefExpr>(innerExpr);
                                            srcForAssign = deref.get();
                                            myBlock->addOwnership(std::move(deref));
                                        } else {
                                            // var is a struct - use directly (no dereference)
                                            srcForAssign = innerExpr;
                                        }
                                    }
                                } else if (auto* srcDeref = llvm::dyn_cast_or_null<DerefExpr>(srcUnwrapped)) {
                                    // Source is already *ptr, use as is
                                    srcForAssign = srcUnwrapped;
                                } else {
                                    // Source is a Value, AggregateElement, or other expression
                                    // Check if it's a pointer type - if so, we need to dereference it
                                    // For memcpy(&ptr, other_ptr, size), we want *ptr = *other_ptr (dereference both)
                                    auto* srcType = srcUnwrapped->getType();
                                    if (auto* ptrType = llvm::dyn_cast_or_null<PointerType>(srcType)) {
                                        // src is a pointer - dereference it to get the struct value
                                        auto deref = std::make_unique<DerefExpr>(srcUnwrapped);
                                        srcForAssign = deref.get();
                                        myBlock->addOwnership(std::move(deref));
                                    } else {
                                        // src is not a pointer, use as is
                                        srcForAssign = srcUnwrapped;
                                    }
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
