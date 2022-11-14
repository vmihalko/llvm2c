#include "../core/Program.h"
#include "SimplifyingExprVisitor.h"

#include <llvm/IR/Instructions.h>

#include <algorithm>
#include <llvm/Analysis/LoopInfo.h>

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
                // Why we have to call accept?
                expr->accept(*this);
                // because we are traversing tree recursively
                // and calling from every node simplify( ... )
                // nothing interesting happens until we found gotoExpr

                // Why simplify is necessary?
                targetBlockExpressions.push_back(simplify(expr));
                // becasue we want to replace goto with block on
                // which is goto pointing...
                // targetBlockExpressions.push_back(expr);
            }

            auto list = std::make_unique<ExprList>(std::move(targetBlockExpressions));
            Expr* result = list.get();
            container->addOwnership(std::move(list));
            return result;
        }
    }

    return expr;
}

// (HEADER --inline-> LATCH) --inline--> PREHEADER
void inlineLoopBlocks(llvm::Loop *loop, Func *fun) {
    if (!loop->isLoopSimplifyForm() || !loop->isRotatedForm() )
        return;

    // move HEAD into LATCH
    auto header = fun->createBlockIfNotExist(loop->getHeader());
    auto headminator = llvm::dyn_cast_or_null<GotoExpr>(header->expressions.back());
    auto latch  = fun->createBlockIfNotExist(loop->getLoopLatch());
    if (headminator->target == latch)
        header->expressions.pop_back();
    auto list = std::make_unique<ExprList>(std::move( header->expressions ));
    Expr* result = list.get();
    latch->addOwnership(std::move(list));

    // replace goto with ExprList
    auto dowhile = llvm::dyn_cast_or_null<DoWhile>(latch->expressions[0]);
    dowhile->body = result;

    auto preHeader = fun->createBlockIfNotExist(loop->getLoopPreheader());
    preHeader->expressions.back() = dowhile;

    preHeader->doInline = false;
    header->doInline = latch->doInline = true;
}

void inlineLoopBlocksInFunction( Func * fun) {
    llvm::LoopInfo &LI = fun->FAM.getResult<llvm::LoopAnalysis>(const_cast<llvm::Function &>(*fun->function));
    for( auto l : LI)
        inlineLoopBlocks(l, fun);
}

void inlineBlocks(const llvm::Module* module, Program& program) {
    assert(program.isPassCompleted(PassType::IdentifyInlinableBlocks));

    for (const llvm::Function& func : module->functions()) {
        auto* function = program.getFunction(&func);
        inlineLoopBlocksInFunction(function);
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
