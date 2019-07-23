#include <llvm/IR/Instruction.h>

#include "../core/Program.h"
#include "../core/Func.h"
#include "../core/Block.h"

#include "SimplifyingExprVisitor.h"

class RefDerefVisitor : public SimplifyingExprVisitor {

protected:
    Expr* simplify(Expr* expr) override;
};

void refDeref(const llvm::Module* module, Program& program) {
    assert(program.isPassCompleted(PassType::CreateExpressions));

    RefDerefVisitor rdv;

    for (const llvm::Function& func : module->functions()) {
        auto* function = program.getFunction(&func);
        for (const auto& block : func) {
            auto* myBlock = function->getBlock(&block);

            for (auto it = myBlock->expressions.begin(); it != myBlock->expressions.end(); ++it) {
                auto expr = *it;
                expr->accept(rdv);
            }
        }
    }

    program.addPass(PassType::RefDeref);
}

Expr* RefDerefVisitor::simplify(Expr* expr) {
    if (auto RE = llvm::dyn_cast_or_null<RefExpr>(expr)) {
        if (auto DE = llvm::dyn_cast_or_null<DerefExpr>(RE->expr)) {
            return DE->expr;
        }
    }

    if (auto DE = llvm::dyn_cast_or_null<DerefExpr>(expr)) {
        if (auto RE = llvm::dyn_cast_or_null<RefExpr>(DE->expr)) {
            return RE->expr;
        }
    }

    return expr;
}
