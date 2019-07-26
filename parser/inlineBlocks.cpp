#include "../core/Program.h"
#include "SimplifyingExprVisitor.h"

#include <llvm/IR/Instructions.h>

#include <algorithm>

class InliningVisitor : public SimplifyingExprVisitor {
private:
    Block* container;
public:
    InliningVisitor(Block* container) : container(container) {}
    Expr* simplify(Expr* expr) override;
};

Expr* InliningVisitor::simplify(Expr* expr) {
    if (auto* gotoExpr = llvm::dyn_cast_or_null<GotoExpr>(expr)) {
        Block* target = gotoExpr->target;

        if (target->doInline) {
            std::vector<Expr*> targetBlockExpressions;
            for (auto* expr : target->expressions) {
                expr->accept(*this);
                targetBlockExpressions.push_back(simplify(expr));
            }

            auto list = std::make_unique<ExprList>(std::move(targetBlockExpressions));
            Expr* result = list.get();
            container->addOwnership(std::move(list));
            return result;
        }
    }

    return expr;
}

void inlineBlocks(const llvm::Module* module, Program& program) {
    assert(program.isPassCompleted(PassType::IdentifyInlinableBlocks));


    for (const llvm::Function& func : module->functions()) {
        auto* function = program.getFunction(&func);
        for (const auto& block : func) {
            auto* myBlock = function->createBlockIfNotExist(&block);
            InliningVisitor iv(myBlock);

            for (auto& expr : myBlock->expressions) {
                expr->accept(iv);
                expr = iv.simplify(expr);
            }
        }
    }

    program.addPass(PassType::InlineBlocks);
}
