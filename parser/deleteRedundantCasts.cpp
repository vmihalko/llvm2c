#include <llvm/IR/Instruction.h>

#include "../core/Program.h"
#include "../core/Func.h"
#include "../core/Block.h"

#include "SimplifyingExprVisitor.h"

class RedundantCastsVisitor: public SimplifyingExprVisitor {
protected:
    Expr* simplify(Expr* expr) override;
};

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
