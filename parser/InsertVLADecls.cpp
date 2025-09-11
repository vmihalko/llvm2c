#include "InsertVLADecls.h"
#include "../core/Func.h"
#include "../core/Block.h"
#include "../expr/Expr.h"
#include "../type/Type.h"

#include "llvm/IR/Instructions.h"
#include "llvm/Support/raw_ostream.h"
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
 
                                            // Remove original declaration locations (prologue vars and block expressions)
                                            if (F) {
                                                // a) prologue variable list
                                                auto &vars = F->variables;
                                                vars.erase(std::remove_if(vars.begin(), vars.end(), [&](Value* V){
                                                    return V == SA->value || V->valueName == SA->value->valueName;
                                                }), vars.end());

                                                // b) any block expression list
                                                for (auto &bpair : F->blockMap) {
                                                    auto &exprs = bpair.second->expressions;
                                                    exprs.erase(std::remove(exprs.begin(), exprs.end(), SA), exprs.end());
                                                }
                                            }
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

// Helper: peel off RefExpr, DerefExpr, and CastExpr to reach underlying operand
static Expr* stripAllWrappers(Expr* e) {
    while (true) {
        if (!e) break;
        if (auto *ref = llvm::dyn_cast<RefExpr>(e)) { e = ref->expr; continue; }
        if (auto *deref = llvm::dyn_cast<DerefExpr>(e)) { e = deref->expr; continue; }
        if (auto *cast = llvm::dyn_cast<CastExpr>(e)) { e = cast->expr; continue; }
        break;
    }
    return e;
}

/* ------------------------------------------------------------------ */
static bool expressionContainsVariable(Expr* expr, const std::string& varName) {
    expr = stripAllWrappers(expr);
    if (!expr) return false;

    if (auto* value = llvm::dyn_cast<Value>(expr)) {
        return value->valueName == varName;
    }
    if (auto* globalValue = llvm::dyn_cast<GlobalValue>(expr)) {
        return globalValue->valueName == varName;
    }
    return false;
}

/* ------------------------------------------------------------------ */
static bool assignsToVariable(Expr* expr, const std::string& varName) {
    if (auto* assignExpr = llvm::dyn_cast<AssignExpr>(expr)) {
        Expr* lhsBase = stripAllWrappers(assignExpr->left);

        if (auto* leftValue = llvm::dyn_cast<Value>(lhsBase)) {
            return leftValue->valueName == varName;
        }
        if (auto* globalValue = llvm::dyn_cast<GlobalValue>(lhsBase)) {
            return globalValue->valueName == varName;
        }
        if (auto *deref = llvm::dyn_cast<DerefExpr>(lhsBase)) {
            lhsBase = stripAllWrappers(deref->expr);
            if (auto *innerVal = llvm::dyn_cast<Value>(lhsBase)) {
                return innerVal->valueName == varName;
            }
            if (auto *innerGV = llvm::dyn_cast<GlobalValue>(lhsBase)) {
                return innerGV->valueName == varName;
            }
        }

        if (expressionContainsVariable(assignExpr->left, varName)) return true;
    }
    if (auto *list = llvm::dyn_cast<ExprList>(expr)) {
        for (auto *sub : list->expressions) {
            if (assignsToVariable(sub, varName)) return true;
        }
    }
    return false;
}

/* ------------------------------------------------------------------ */
static std::string getSizeVariableName(Expr* sizeExpr) {
    while (sizeExpr) {
        if (auto *value = llvm::dyn_cast<Value>(sizeExpr)) {
            return value->valueName;
        }
        if (auto *gv = llvm::dyn_cast<GlobalValue>(sizeExpr)) {
            return gv->valueName;
        }
        if (auto *ref = llvm::dyn_cast<RefExpr>(sizeExpr)) {
            sizeExpr = ref->expr; continue;
        }
        if (auto *deref = llvm::dyn_cast<DerefExpr>(sizeExpr)) {
            sizeExpr = deref->expr; continue;
        }
        if (auto *cast = llvm::dyn_cast<CastExpr>(sizeExpr)) {
            sizeExpr = cast->expr; continue;
        }
        break;
    }
    return ""; // Unknown/complex expression
}

/* ------------------------------------------------------------------ */
static void removeOriginalVLADecls(Func* F, const Map &M) {
    // Remove StackAlloc nodes from basic blocks
    for (auto &BP : F->blockMap) {
        auto &vec = BP.second->expressions;
        vec.erase(std::remove_if(vec.begin(), vec.end(), [&](Expr* E) {
            for (const auto &pair : M) {
                if (E == pair.second.stackAlloc) return true;
            }
            return false;
        }), vec.end());
    }

    // Remove corresponding Value objects from the prologue variable list so writer will not emit them upfront
    auto &vars = F->variables;
    vars.erase(std::remove_if(vars.begin(), vars.end(), [&](Value* V){
        for (const auto &pair : M) {
            if (pair.second.stackAlloc->value == V) return true;
            if (pair.second.stackAlloc->value->valueName == V->valueName) return true;
        }
        return false;
    }), vars.end());
}

/* ------------------------------------------------------------------ */
static void insertAfterSizeInitialization(Func *F, Map &M)
{
    if (M.empty()) return;

    // First, get rid of the original premature declarations
    removeOriginalVLADecls(F, M);
    
    // Strategy: Insert VLA declarations after size variable assignments
    // This ensures proper initialization order for 150,000+ benchmarks
    
    for (auto &BP : F->blockMap) {
        auto &vec = BP.second->expressions;
        
        for (auto it = vec.begin(); it != vec.end(); ++it) {
            Expr *E = *it;

#ifdef VLA_DEBUG
            if (auto *AE = llvm::dyn_cast<AssignExpr>(E)) {
                // Helper to strip wrappers
                auto strip = [](Expr* ex) -> Expr* {
                    while (true) {
                        if (auto *ref = llvm::dyn_cast<RefExpr>(ex)) { ex = ref->expr; continue; }
                        if (auto *deref = llvm::dyn_cast<DerefExpr>(ex)) { ex = deref->expr; continue; }
                        if (auto *cast = llvm::dyn_cast<CastExpr>(ex)) { ex = cast->expr; continue; }
                        break;
                    }
                    return ex;
                };

                Expr* lhsBase = strip(AE->left);
                llvm::errs() << "[VLA_DEBUG] AssignExpr LHS kind=" << lhsBase->getKind();
                if (auto *v = llvm::dyn_cast<Value>(lhsBase)) {
                    llvm::errs() << " name=" << v->valueName;
                } else if (auto *gv = llvm::dyn_cast<GlobalValue>(lhsBase)) {
                    llvm::errs() << " gname=" << gv->valueName;
                }
                llvm::errs() << "\n";
            }
#endif
            
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
