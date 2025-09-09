#include "InsertVLADecls.h"
#include "../core/Func.h"
#include "../core/Block.h"
#include "../expr/Expr.h"
#include "../type/Type.h"

#include "llvm/IR/Instructions.h"
#include <map>

// Map from the RefExpr (created for the AllocaInst) to the StackAlloc and size variable info
struct VLAInfo {
    StackAlloc* stackAlloc;
    Expr* sizeExpr;  // The expression that represents the size (e.g., CELLCOUNT)
};
using Map = std::map<RefExpr*, VLAInfo>;

/* ------------------------------------------------------------------ */
static void collectVLAs(Func *F, Map &M, Program &P)
{
    // Find VLA AllocaInsts and their corresponding StackAllocs
    
    for (auto &pair : P.exprMap) {
        const llvm::Value *llvmVal = pair.first;
        Expr *expr = pair.second;
        
        // Check if this is a VLA AllocaInst
        if (llvmVal) {
            if (auto *AI = llvm::dyn_cast<llvm::AllocaInst>(llvmVal)) {
                if (AI->isArrayAllocation() && !llvm::isa<llvm::ConstantInt>(AI->getArraySize())) {
                    // The expr should be a RefExpr pointing to the VLA variable
                    if (auto *refExpr = llvm::dyn_cast<RefExpr>(expr)) {
                        // Find the corresponding StackAlloc in the ownership list
                        for (auto &ownedExpr : P.ownership) {
                            if (auto *SA = llvm::dyn_cast<StackAlloc>(ownedExpr.get())) {
                                if (SA->value == refExpr->expr) {
                                    // Get the size expression from the VLA type
                                    if (auto *arrayTy = llvm::dyn_cast<ArrayType>(SA->value->getType())) {
                                        if (arrayTy->dynSize) {
                                            VLAInfo info;
                                            info.stackAlloc = SA;
                                            info.sizeExpr = arrayTy->dynSize;
                                            M[refExpr] = info;
                                            break;
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

/* ------------------------------------------------------------------ */
static bool dependsOnVariable(Expr* expr, const std::string& varName) {
    if (!expr) return false;
    
    if (auto* value = llvm::dyn_cast<Value>(expr)) {
        return value->valueName == varName;
    }
    if (auto* refExpr = llvm::dyn_cast<RefExpr>(expr)) {
        return dependsOnVariable(refExpr->expr, varName);
    }
    if (auto* assignExpr = llvm::dyn_cast<AssignExpr>(expr)) {
        return dependsOnVariable(assignExpr->left, varName) || dependsOnVariable(assignExpr->right, varName);
    }
    if (auto* callExpr = llvm::dyn_cast<CallExpr>(expr)) {
        // Check if any parameter depends on the variable
        for (auto* param : callExpr->params) {
            if (dependsOnVariable(param, varName)) {
                return true;
            }
        }
    }
    // Add more expression types as needed for robustness
    return false;
}

/* ------------------------------------------------------------------ */
static bool expressionContainsVariable(Expr* expr, const std::string& varName) {
    if (!expr) return false;
    
    if (auto* value = llvm::dyn_cast<Value>(expr)) {
        return value->valueName == varName;
    }
    if (auto* refExpr = llvm::dyn_cast<RefExpr>(expr)) {
        return expressionContainsVariable(refExpr->expr, varName);
    }
    if (auto* globalValue = llvm::dyn_cast<GlobalValue>(expr)) {
        return globalValue->valueName == varName;
    }
    // Add more expression types as needed
    return false;
}

/* ------------------------------------------------------------------ */
static bool assignsToVariable(Expr* expr, const std::string& varName) {
    if (auto* assignExpr = llvm::dyn_cast<AssignExpr>(expr)) {
        // Direct assignment to a Value (local variable)
        if (auto* leftValue = llvm::dyn_cast<Value>(assignExpr->left)) {
            return leftValue->valueName == varName;
        }
        // Assignment through a RefExpr (global variable or pointer dereference)  
        else if (auto* refExpr = llvm::dyn_cast<RefExpr>(assignExpr->left)) {
            if (auto* innerValue = llvm::dyn_cast<Value>(refExpr->expr)) {
                return innerValue->valueName == varName;
            }
        }
        // Try other expression types for global variables
        else if (auto* globalValue = llvm::dyn_cast<GlobalValue>(assignExpr->left)) {
            return globalValue->valueName == varName;
        }
        // Fallback: check if the variable name appears anywhere in the left side
        else if (expressionContainsVariable(assignExpr->left, varName)) {
            return true;
        }
    }
    return false;
}

/* ------------------------------------------------------------------ */
static std::string getSizeVariableName(Expr* sizeExpr) {
    if (auto* value = llvm::dyn_cast<Value>(sizeExpr)) {
        return value->valueName;
    }
    if (auto* refExpr = llvm::dyn_cast<RefExpr>(sizeExpr)) {
        return getSizeVariableName(refExpr->expr);
    }
    return ""; // Unknown/complex expression
}

/* ------------------------------------------------------------------ */
static void insertAfterSizeInitialization(Func *F, Map &M)
{
    if (M.empty()) return;
    
    // Strategy: Insert VLA declarations after size variable assignments
    // This ensures proper initialization order for 150,000+ benchmarks
    
    for (auto &BP : F->blockMap) {
        auto &vec = BP.second->expressions;
        
        for (auto it = vec.begin(); it != vec.end(); ++it) {
            Expr *E = *it;
            
            // Check if this expression assigns to any VLA size variable
            for (auto mit = M.begin(); mit != M.end(); ) {
                VLAInfo &info = mit->second;
                std::string sizeVarName = getSizeVariableName(info.sizeExpr);
                
                bool foundSizeAssignment = false;
                
                if (!sizeVarName.empty() && assignsToVariable(E, sizeVarName)) {
                    foundSizeAssignment = true;
                }
                
                if (foundSizeAssignment) {
                    // Insert the VLA declaration right after this assignment
                    vec.insert(it + 1, info.stackAlloc);
                    M.erase(mit++);
                    break; // Only process one VLA per expression to avoid iterator issues
                } else {
                    ++mit;
                }
            }
        }
    }
    
    // Fallback: For any remaining VLAs, insert them at the beginning of the first block
    // This handles cases where we couldn't find the size assignment
    if (!M.empty()) {
        Block* entryBlock = nullptr;
        for (auto &BP : F->blockMap) {
            if (!BP.second->expressions.empty()) {
                entryBlock = BP.second.get();
                break;
            }
        }
        
        if (entryBlock) {
            auto insertPos = entryBlock->expressions.begin();
            for (auto &pair : M) {
                VLAInfo &info = pair.second;
                entryBlock->expressions.insert(insertPos, info.stackAlloc);
            }
        }
    }
    
    M.clear();
}

/* ------------------------------------------------------------------ */
void insertVLADecls(Program &P)
{
    for (auto &FP : P.functions) {
        auto *F = FP.second.get();
        if (F->isDeclaration) continue;

        Map pending;
        collectVLAs(F, pending, P);
        insertAfterSizeInitialization(F, pending);
    }
}
