#include <cerrno>
#include <cstdlib>
#include <llvm/IR/Instruction.h>

#include "../core/Program.h"
#include "../core/Func.h"
#include "../core/Block.h"

#include "SimplifyingExprVisitor.h"

class RedundantCastsVisitor: public SimplifyingExprVisitor {
protected:
    Expr* simplify(Expr* expr) override;
};

// Concrete integer kinds whose C spelling is fully determined by
// (kind, unsignedType). Excludes TK_BoolType (bool casts normalize values)
// and the generic named TK_IntegerType (spelling lives in a private name).
static bool isConcreteIntKind(Type::TypeKind kind) {
    switch (kind) {
    case Type::TK_CharType:
    case Type::TK_IntType:
    case Type::TK_ShortType:
    case Type::TK_LongType:
    case Type::TK_LongLongType:
    case Type::TK_Int128:
        return true;
    default:
        return false;
    }
}

// (T)x is an identity cast iff x's static type is already exactly T.
// Type* identity is unreliable here (TypeHandler holds separate caches for
// llvm types and debug-info types, plus copies), so compare structurally:
// same integer kind and same signedness. Generic named integer types must
// also match in spelling.
static bool isIdentityIntCast(Type* dst, Type* src) {
    auto* dstInt = llvm::dyn_cast_or_null<IntegerType>(dst);
    auto* srcInt = llvm::dyn_cast_or_null<IntegerType>(src);
    if (!dstInt || !srcInt)
        return false;
    if (llvm::isa<BoolType>(dstInt) || llvm::isa<BoolType>(srcInt))
        return false;
    if (dstInt->getKind() != srcInt->getKind())
        return false;
    if (dstInt->unsignedType != srcInt->unsignedType)
        return false;
    if (!isConcreteIntKind(dstInt->getKind()))
        return dstInt->toString() == srcInt->toString();
    return true;
}

// The identity test above compares against getType(), but the C type of the
// EMITTED operand is what decides the semantics of the enclosing expression.
// The two provably agree only for operands whose printed form carries its own
// type: a cast (prints "(S)y", C type S == getType()), a named variable
// (prints its identifier, C type == the declaration, which the writer prints
// from the same Type* object), or an unsuffixed decimal literal that fits in
// 'int' when the target is plain signed int (the literal's C type is 'int'
// by [lex.icon], independent of getType()). For anything else the C type is
// derived by usual arithmetic conversions over subexpressions (BinaryExpr,
// SelectExpr, ...) or by a literal spelling that does not match the recorded
// type (e.g. a negative constant recorded as unsigned), so getType() may
// disagree with the emitted type and the cast must be kept.
static bool emittedTypeMatchesRecorded(Expr* expr) {
    if (llvm::isa<CastExpr>(expr))
        return true;
    if (auto* val = llvm::dyn_cast<Value>(expr)) {
        const std::string& name = val->valueName;
        if (name.empty())
            return false;
        char c = name[0];
        return c == '_' || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
    }
    return false;
}

// (int)LIT where LIT is an unsuffixed non-negative decimal literal that fits
// in 'int': the emitted literal already has C type 'int' ([lex.icon]) and the
// same value, independent of the type recorded for it (constants are often
// interned with an unsigned type), so the cast is redundant in the emitted C.
// Only for target 'int': for any other target the bare literal would change
// the C type of the operand. Negative spellings are excluded so the drop can
// never interact with contexts that read the operand's recorded signedness
// (e.g. the shift writer).
static bool isPlainIntLiteralCast(Type* dst, Expr* operand) {
    auto* dstInt = llvm::dyn_cast_or_null<IntegerType>(dst);
    if (!dstInt || llvm::isa<BoolType>(dstInt))
        return false;
    if (dstInt->getKind() != Type::TK_IntType || dstInt->unsignedType)
        return false;
    auto* val = llvm::dyn_cast_or_null<Value>(operand);
    if (!val || llvm::isa<GlobalValue>(val))
        return false;
    if (!llvm::isa<IntegerType>(val->getType()) ||
        llvm::isa<BoolType>(val->getType()))
        return false;
    const std::string& name = val->valueName;
    if (name.empty() || name.size() > 10)
        return false;
    for (char c : name)
        if (c < '0' || c > '9')
            return false;
    errno = 0;
    char* end = nullptr;
    long long v = std::strtoll(name.c_str(), &end, 10);
    return errno == 0 && end && *end == '\0' && v <= 2147483647LL;
}

void deleteRedundantCasts(const llvm::Module* module, Program& program) {
    assert(program.isPassCompleted(PassType::CreateExpressions));

    RedundantCastsVisitor rcv;

    for (const llvm::Function& func : module->functions()) {
        auto* function = program.getFunction(&func);
        for (const auto& block : func) {
            auto* myBlock = function->getBlock(&block);

            for (auto it = myBlock->expressions.begin(); it != myBlock->expressions.end(); ++it) {
                auto expr = *it;
                expr->accept(rcv);

            }
        }
    }
    program.addPass(PassType::DeleteRedundantCasts);
}


Expr* RedundantCastsVisitor::simplify(Expr* expr) {
    // Collapse identical cast chains: (T)(T)(X) => (T)(X)
    if (auto* outer = llvm::dyn_cast_or_null<CastExpr>(expr)) {
        if (auto* inner = llvm::dyn_cast_or_null<CastExpr>(outer->expr)) {
            if (inner->getType() == outer->getType()) {
                outer->expr = inner->expr;
            }
        }

        // Drop integer-of-bool chains: (unsigned int)((_Bool)(X)) => ((_Bool)(X))
        // and (unsigned int)(BoolExpr) => BoolExpr
        if (llvm::isa<IntegerType>(outer->getType())) {
            // Case 1: integer-of-(cast-to-bool)
            if (auto* innerBoolCast = llvm::dyn_cast_or_null<CastExpr>(outer->expr)) {
                if (llvm::isa<BoolType>(innerBoolCast->getType())) {
                    return innerBoolCast; // keep only the bool cast
                }
            }
            // Case 2: integer-of-boolean-typed expression (cmp/logical/bool)
            Expr* innerExpr = outer->expr;
            if (llvm::isa<CmpExpr>(innerExpr) ||
                llvm::isa<LogicalAnd>(innerExpr) ||
                llvm::isa<LogicalOr>(innerExpr) ||
                llvm::isa<LogicalNot>(innerExpr) ||
                llvm::isa<BoolType>(innerExpr->getType())) {
                return innerExpr; // drop the outer integer cast entirely
            }
        }

        // Drop redundant bool casts of boolean expressions:
        // (_Bool)(cmp/logical/bool) => cmp/logical/bool
        if (llvm::isa<BoolType>(outer->getType())) {
            Expr* innerExpr = outer->expr;
            if (llvm::isa<CmpExpr>(innerExpr) ||
                llvm::isa<LogicalAnd>(innerExpr) ||
                llvm::isa<LogicalOr>(innerExpr) ||
                llvm::isa<LogicalNot>(innerExpr) ||
                llvm::isa<BoolType>(innerExpr->getType())) {
                return innerExpr;
            }
        }

        // Drop identity casts: (T)x => x when x already has type T
        // ("drops every cast whose operand already has the target type").
        // Only for operands whose emitted C type provably equals getType();
        // when the emitted type is uncertain, keep the cast.
        if (outer->expr &&
            isIdentityIntCast(outer->getType(), outer->expr->getType()) &&
            emittedTypeMatchesRecorded(outer->expr)) {
            return outer->expr;
        }

        // Drop (int)LIT for plain in-range decimal literals (see above).
        if (outer->expr && isPlainIntLiteralCast(outer->getType(), outer->expr)) {
            return outer->expr;
        }
    }
    // Simplify comparisons with symmetric identical integer casts:
    // (uint8)A == (uint8)B  => A == B    (when both casts only normalize width/sign)
    // For unsigned comparisons, also simplify ordered comparisons:
    // (T_large)((T_small)X) op (T_large)(expr) => (T_small)X op (expr without outer cast)
    // where op is >, >=, <, <= and both casts are zero-extensions (unsigned to unsigned)
    if (auto* cmp = llvm::dyn_cast_or_null<CmpExpr>(expr)) {
        auto* leftCast  = llvm::dyn_cast_or_null<CastExpr>(cmp->left);
        auto* rightCast = llvm::dyn_cast_or_null<CastExpr>(cmp->right);
        
        if (leftCast && rightCast) {
            // Both sides are casts; check if target types are equal integer types
            auto* leftTy  = leftCast->getType();
            auto* rightTy = rightCast->getType();
            
            if (leftTy == rightTy && llvm::isa<IntegerType>(leftTy)) {
                auto* leftIntTy = llvm::dyn_cast<IntegerType>(leftTy);
                auto* leftSrcTy = llvm::dyn_cast_or_null<IntegerType>(leftCast->expr->getType());
                
                // For equality/inequality, simplify if both sides are direct casts from integer types
                if (cmp->comparsion == "==" || cmp->comparsion == "!=") {
                    auto* rightSrcTy = llvm::dyn_cast_or_null<IntegerType>(rightCast->expr->getType());
                    if (leftSrcTy && rightSrcTy) {
                        cmp->left  = leftCast->expr;
                        cmp->right = rightCast->expr;
                        return expr;
                    }
                }
                
                // For ordered comparisons, simplify if:
                // 1. The comparison is unsigned (zero-extension semantics)
                // 2. Both sides are casts from the same source type
                // 3. Both outer casts are unsigned (zero-extensions)
                // 4. Both source types are unsigned integer types
                // For unsigned comparisons, removing the outer casts is safe because
                // zero-extension preserves the ordering relationship, but only if
                // both sides have the same source type to maintain type compatibility
                if (cmp->isUnsigned && 
                    leftSrcTy && 
                    leftSrcTy->unsignedType && 
                    leftIntTy->unsignedType) {
                    auto* rightSrcTy = llvm::dyn_cast_or_null<IntegerType>(rightCast->expr->getType());
                    
                    // Only simplify if both source types are the same unsigned integer type
                    // This ensures type compatibility after removing the casts
                    if (rightSrcTy && rightSrcTy->unsignedType && leftSrcTy == rightSrcTy) {
                        // Remove the outer casts from both sides
                        cmp->left = leftCast->expr;
                        cmp->right = rightCast->expr;
                        return expr;
                    }
                }
            }
        }
    }
    return expr;
}
