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
                targetBlockExpressions.push_back( simplify(expr) );
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
#include <iostream>
// (HEADER --inline-> LATCH) --inline--> PREHEADER
void inlineLoopBlocks(llvm::Loop *loop, Func *fun) {
    llvm::errs() << "----preeader----"
                 << *loop->getLoopPreheader()
                 << "\n----header----"
                 << *loop->getHeader()
                 << "\n----latch----"
                 << *loop->getLoopLatch()    
                 << "\n----latch----\n";
    if (!loop->isLoopSimplifyForm())
        llvm::errs() << "simplified\n";
    if (!loop->isRotatedForm() )
        llvm::errs() << "roated\n";




    // move HEAD into LATCH 
    // IF not header == latch
    auto header = fun->createBlockIfNotExist(loop->getHeader());
    // llvm::errs() << header->expressions.size();
    auto latch  = fun->createBlockIfNotExist(loop->getLoopLatch());
    auto preHeader = fun->createBlockIfNotExist(loop->getLoopPreheader());
    preHeader->doInline = true;
    header->doInline = true;
    latch->doInline = true;
    // preHeader->doInline = false;
    // header->doInline = false;
    // latch->doInline = false;
    return;
    if ( ! (header == latch) )  {
        // Find edge to latch (dfs?) [example if inside for cycle?]
        auto headminator = llvm::dyn_cast_or_null<GotoExpr>(header->expressions.back());
        if (headminator->target == latch)
            header->expressions.pop_back();
        auto list = std::make_unique<ExprList>(std::move( header->expressions ));
        Expr* result = list.get();
        latch->addOwnership(std::move(list));

    } // header and latch are the same thing
    if (header == latch) {
        
        for (auto e : latch->expressions) {
        std::cout << e->getKind() << std::endl;
        switch (e->getKind())
        {
        case Expr::EK_AssignExpr:
            std::cout << "ass\n";
            break;
        case Expr::EK_GotoExpr:
            // auto* gotoExpr = ;   
            std::cout << "goto: " << llvm::dyn_cast_or_null<GotoExpr>(e)->target->blockName << std::endl;
            break;
        case Expr::EK_DoWhile:
            std::cout << "dwhile\n";
            break;
        case Expr::EK_RetExpr:
            std::cout << "ret\n";
            break;
        default:
            break;
        }
        }

        auto it = std::find_if(latch->expressions.begin(), latch->expressions.end(),
            [](Expr *e){return llvm::dyn_cast_or_null<DoWhile>(e);});

        if (latch->expressions.end() == it)
            assert(0 && "Cannot find doWhile");

        auto *dowhile = llvm::dyn_cast_or_null<DoWhile>(*it);
        if (!dowhile) {     
            exit(254);
        }

        //get gotoExpr from doWhile0
        // auto *exprList = llvm::dyn_cast_or_null<ExprList *>(dowhile->body());
        // exprL
        // auto itt = std::find_if(latch->expressions.begin(), latch->expressions.end(),
        //     [](Expr *e){return llvm::dyn_cast_or_null<GotoExpr>(e);});

        // preHeader->expressions.back() = dowhile;

        latch->doInline = false;
        }
        
}

void inlineLoopBlocksInFunction( Func * fun) {
    llvm::LoopInfo &LI = fun->FAM.getResult<llvm::LoopAnalysis>(const_cast<llvm::Function &>(*fun->function));
    for( auto l : LI)
        inlineLoopBlocks(l, fun);
}

void doNotInlineSubloopLatchBlockIntoGotoLatch(Func * fun, llvm::Loop *loop) {
    fun->getBlock( loop->getLoopLatch() )->doInline = loop->getLoopLatch() == loop->getHeader() ||
                                                        ( (loop->getHeader()->getTerminator()->getSuccessor(0) == loop->getLoopLatch()) &&
            loop->getLoopLatch()->getSinglePredecessor() != nullptr /*loopHeader --SINGLE-EDGE--> loopLatch*/ );

    for(auto subLoop : loop->getSubLoops()){
        doNotInlineSubloopLatchBlockIntoGotoLatch( fun, subLoop );
    }

}

void doNotInlineLatchBlocksIntoGotoLatch(Func * fun) {
    llvm::LoopInfo &LI = fun->FAM.getResult<llvm::LoopAnalysis>(const_cast<llvm::Function &>(*fun->function));
    for( auto l : LI)
        doNotInlineSubloopLatchBlockIntoGotoLatch(fun, l);
}

void markLatchBlockAsInlined(Func * fun, llvm::Loop *loop) {
    fun->getBlock( loop->getLoopLatch() )->doInline = true;
    for(auto subLoop : loop->getSubLoops()){
        markLatchBlockAsInlined( fun, subLoop );
    }

}

void markLatchBlockAsInlined(Func *fun) /*fool writer*/ {
    llvm::LoopInfo &LI = fun->FAM.getResult<llvm::LoopAnalysis>(const_cast<llvm::Function &>(*fun->function));
    for( auto l : LI)
        markLatchBlockAsInlined(fun, l);
}


void inlineBlocks(const llvm::Module* module, Program& program) {
    assert(program.isPassCompleted(PassType::IdentifyInlinableBlocks));

    for (const llvm::Function& func : module->functions()) {
        if (func.isIntrinsic())
            continue;
        auto* function = program.getFunction(&func);
        // inlineLoopBlocksInFunction(function);
        doNotInlineLatchBlocksIntoGotoLatch(function);
        for (const auto& block : func) {
            auto* myBlock = function->createBlockIfNotExist(&block);
            InliningVisitor iv(myBlock);

            for (auto& expr : myBlock->expressions) {
                expr->accept(iv);
                expr = iv.simplify(expr);
            }
        }
        markLatchBlockAsInlined(function);
    }

    program.addPass(PassType::InlineBlocks);
}
