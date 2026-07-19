#include <llvm/IR/Instruction.h>

#include "../core/Program.h"
#include "../core/Func.h"
#include "../core/Block.h"
#include "../expr/Expr.h"

#include <unordered_set>
#include <algorithm>

/**
 * This pass fixes the types for overflow intrinsic structs.
 * 
 * Problem: LLVM overflow intrinsics like llvm.sadd.with.overflow.i32 return a struct
 * {i32, i1}. Since LLVM IR doesn't distinguish signed/unsigned at the type level,
 * llvm2c defaults to unsigned types. But for signed intrinsics (sadd, ssub, smul),
 * the result should be signed.
 * 
 * Solution: After expressions are created but BEFORE propagateTypes, identify structs
 * used by signed overflow intrinsics and fix their first member to be signed.
 * Then propagateTypes will naturally propagate the correct signed types to variables.
 * 
 * Must run: after createExpressions, before propagateTypes
 */

// Check if an intrinsic name is a signed overflow intrinsic
static bool isSignedOverflowIntrinsic(const std::string& name) {
    return name.find("sadd") != std::string::npos ||
           name.find("ssub") != std::string::npos ||
           name.find("smul") != std::string::npos;
}

// Get the signed equivalent of an unsigned integer type
static IntegerType* getSignedEquivalent(IntegerType* unsignedType, TypeHandler& handler) {
    if (!unsignedType || !unsignedType->unsignedType) {
        return unsignedType; // Already signed or not an integer
    }
    
    // Check by type string to determine bit width
    std::string typeStr = unsignedType->toString();
    if (typeStr == "unsigned char") return handler.schar.get();
    if (typeStr == "unsigned short") return handler.sshort.get();
    if (typeStr == "unsigned int") return handler.sint.get();
    if (typeStr == "unsigned long long") return handler.slonglong.get();
    return unsignedType; // No signed equivalent found
}

void fixOverflowIntrinsicTypes(const llvm::Module* /*module*/, Program& program) {
    std::unordered_set<StructType*> signedOverflowStructs;
    
    // Identify structs used by signed overflow intrinsics
    for (const auto& intrinsicName : program.intrinsicsNeedingDefinitions) {
        if (!isSignedOverflowIntrinsic(intrinsicName)) continue;
        
        std::string normalizedName = intrinsicName;
        std::replace(normalizedName.begin(), normalizedName.end(), '.', '_');
        
        for (const auto& funcPair : program.functions) {
            const Func* func = funcPair.second.get();
            for (const auto& blockPair : func->blockMap) {
                const Block* block = blockPair.second.get();
                for (Expr* expr : block->expressions) {
                    // Check CallExpr directly and in AssignExpr
                    CallExpr* call = nullptr;
                    if (auto* c = llvm::dyn_cast_or_null<CallExpr>(expr)) {
                        call = c;
                    } else if (auto* assign = llvm::dyn_cast_or_null<AssignExpr>(expr)) {
                        call = llvm::dyn_cast_or_null<CallExpr>(assign->right);
                    }
                    
                    if (call) {
                        std::string callName = call->funcName;
                        std::replace(callName.begin(), callName.end(), '.', '_');
                        
                        if (callName == normalizedName) {
                            Type* returnType = call->getType();
                            if (auto* st = llvm::dyn_cast_or_null<StructType>(returnType)) {
                                signedOverflowStructs.insert(st);
                            }
                        }
                    }
                }
            }
        }
    }
    
    if (signedOverflowStructs.empty()) return;
    
    // Fix struct member types - first member should be signed for signed intrinsics
    for (StructType* st : signedOverflowStructs) {
        if (st->items.empty()) continue;
        
        Type* firstMemberType = st->items[0].first;
        if (auto* intType = llvm::dyn_cast_or_null<IntegerType>(firstMemberType)) {
            IntegerType* signedType = getSignedEquivalent(intType, program.typeHandler);
            if (signedType != intType) {
                st->items[0].first = signedType;
            }
        }
    }
    
    // propagateTypes will handle fixing variable types based on the corrected struct
    
    program.addPass(PassType::FixOverflowIntrinsicTypes);
}

