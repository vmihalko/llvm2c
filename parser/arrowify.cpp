#include <llvm/IR/Instruction.h>

#include "../core/Program.h"
#include "../core/Func.h"
#include "../core/Block.h"

#include "SimplifyingExprVisitor.h"

class ArrowifyVisitor : public SimplifyingExprVisitor {
private:
    Program& program;
protected:
    Expr* simplify(Expr* expr) override;
public:
    ArrowifyVisitor(Program& program): program(program) {}
};

void arrowify(const llvm::Module* module, Program& program) {
    assert(program.isPassCompleted(PassType::CreateExpressions));

    ArrowifyVisitor visitor{program};
    for (const llvm::Function& func : module->functions()) {
        auto* function = program.getFunction(&func);
        for (const auto& block : func) {
            auto* myBlock = function->getBlock(&block);

            for (auto it = myBlock->expressions.begin(); it != myBlock->expressions.end(); ++it) {
                auto expr = *it;
                expr->accept(visitor);

            }

        }
    }

    program.addPass(PassType::Arrowify);
}

Expr* ArrowifyVisitor::simplify(Expr* expr) {

    if (auto* SE = llvm::dyn_cast_or_null<AggregateElement>(expr)) {
        if (auto* DE = llvm::dyn_cast_or_null<DerefExpr>(SE->expr)) {
            return program.makeExpr<ArrowExpr>(DE->expr, SE->element);
        }
    }

    return expr;
}
