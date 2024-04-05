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
    if (auto* cast = llvm::dyn_cast_or_null<CastExpr>(expr)) {
        Expr* innermost = cast->expr;

        while (auto* inner = llvm::dyn_cast_or_null<CastExpr>(innermost)) {
            // cast with love...
            if ( inner->isLossy() )
                break;
            innermost = inner->expr;
        }

        cast->expr = innermost;
    }
    return expr;
}
