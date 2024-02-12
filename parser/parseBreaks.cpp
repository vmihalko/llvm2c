#include "../core/Program.h"
#include "../core/Func.h"
#include "../core/Block.h"
#include "cfunc.h"
#include "constval.h"
#include "negCmpInst.h"
#include "compare.h"
#include <algorithm>

#include <llvm/IR/Instruction.h>
#include <llvm/Analysis/LoopAnalysisManager.h>
#include <llvm/Analysis/FunctionPropertiesAnalysis.h>
#include <llvm/Passes/PassBuilder.h>
#include <llvm/Analysis/LoopInfo.h>

#include <iterator>
/**
 * return phi assignments as ExprList
 */
static std::vector<Expr*> generatePhiAssignments(Block* blockEnding, Block* nextBlock) {
    std::vector<Expr*> exprs;

    Program& program = *blockEnding->func->program;

    for (const auto& phi : nextBlock->block->phis()) {
        Expr* phiVar = program.getExpr(&phi);

        // see if nextBlock declares any incoming value from blockEnding
        for (unsigned i = 0; i < phi.getNumIncomingValues(); ++i) {
            if (phi.getIncomingBlock(i) == blockEnding->block) {
                Expr* incoming = program.getExpr(phi.getIncomingValue(i));
                exprs.push_back(program.makeExpr<AssignExpr>(phiVar, incoming));
            }
        }
    }

    return exprs;
}

static Expr* createListOfOneGoto(Block* container, Block* gotoTarget, bool isLoop = false) {
    auto gotoExpr = std::make_unique<GotoExpr>(gotoTarget);

    std::vector<Expr*> exprs = generatePhiAssignments(container, gotoTarget);
    exprs.push_back(gotoExpr.get());
    if (isLoop)
        std::rotate(exprs.rbegin(), exprs.rbegin() + 1, exprs.rend());
    auto list = std::make_unique<ExprList>(std::move(exprs));

    container->addOwnership(std::move(gotoExpr));
    auto* result = list.get();
    container->addOwnership(std::move(list));
    return result;
}

static void parseUncondBranch(const llvm::Instruction& ins, Func* func, Block* block) {
        Block* target = func->createBlockIfNotExist((llvm::BasicBlock*)ins.getOperand(0));

        auto assignments = generatePhiAssignments(block, target);
        for (auto& assignment : assignments) {
            block->addExpr(assignment);
        }

        auto gotoExpr = std::make_unique<GotoExpr>(target);
        func->program->exprMap[&ins] = gotoExpr.get();
        block->addExprAndOwnership(std::move(gotoExpr));
        block->brHandled = true;
}
/*
void parseComplexLoop(const llvm::BranchInst& ins, Func* func, Block* block, llvm::Loop *loop) {
    // This is the doWhile condition
    // Expr* doWhileCondExpr = func->getExpr(ins.getCondition());
    // Block* HeaderBlock = func->getBlock( loop->getHeader() );
    // HeaderBlock->expressions;


    //create doWhile node
    Block* loopNodes = func->createBlockIfNotExist((llvm::BasicBlock*)ins.getOperand(2));
    auto doWhileBody = std::make_unique<ExprList>(std::move( loopNodes->expressions ));
    // Expr *doWhileBodyPtr = doWhileBody.get();
    // merge eveything into doWhile body
    return;
}
*/
Expr *parseRETURN(const llvm::ReturnInst& retIns, Func* func/*, Block *block*/) {
    if ( !retIns.getReturnValue() ) {
        func->createExpr(&retIns, std::make_unique<RetExpr>());
    } else {
        func->createExpr(&retIns, std::make_unique<RetExpr>(func->getExpr(retIns.getReturnValue())));
    }
    return func->getExpr(&retIns);
}
Expr *parseIF(const llvm::BranchInst& ins, Func* func, Block* block) {
        Expr* cmp = func->getExpr(ins.getCondition());

        Block* falseBlock = func->createBlockIfNotExist((llvm::BasicBlock*)ins.getOperand(1));
        Block* trueBlock = func->createBlockIfNotExist((llvm::BasicBlock*)ins.getOperand(2));

        auto ifExpr = std::make_unique<IfExpr>(cmp, createListOfOneGoto(block, trueBlock), 
                                                    createListOfOneGoto(block, falseBlock));
        func->createExpr(&ins, std::move(ifExpr));
    return func->getExpr(&ins);
}

Expr *parseBlock( Func* func, const llvm::BasicBlock *block) {
    // return func->entry->expressions[0];
    const llvm::Instruction *blockTerminator = block->getTerminator();
 // const llvm::CastInst* CI = llvm::cast<const llvm::CastInst>(&ins);
    auto *currBlock = func->getBlock(block);

    Expr *terExpr = nullptr;
    if (auto returnInst = llvm::dyn_cast<llvm::ReturnInst>(blockTerminator)) {
        terExpr = parseRETURN(*returnInst, func/*, block*/);
        currBlock->addExpr(terExpr);
        currBlock->brHandled = true;
    }
    else if (auto brInst = llvm::dyn_cast<llvm::BranchInst>(blockTerminator)) {
        if ( brInst->isConditional() ) {
            terExpr = parseIF(*brInst, func, currBlock);
            currBlock->addExpr(terExpr);
            currBlock->brHandled = true;

        } else {
            parseUncondBranch(*blockTerminator, func, currBlock); // this function process and add EXPR into <currBlock>
        }
    }

    return terExpr;
}

void parseLoop(Func* func, const llvm::Loop *loop) {
    for( const auto& subLoop : loop->getSubLoops())
        parseLoop( func, subLoop );
    // ------------------------------------------------------------------------------------

    if ( !loop->isRotatedForm()) {
        for( const auto& loopBodyBlock : loop->blocks() ) {
            if (!func->getBlock(loopBodyBlock)->brHandled)
                parseBlock(func, loopBodyBlock);
        }
        return;
    }
    // preheader is not yet processed...process it!
    parseBlock( func, loop->getLoopPreheader() );
    // preheader is processed

    // get the loop pre header terminator
    // from definition, this should be, unconditional br inst into the loop header
    auto llLoopPreheader = loop->getLoopPreheader();
    auto loopPreheader = func->getBlock( llLoopPreheader );
    auto loopPreheaderTerminator = loopPreheader->expressions.back();
    
    // sanity check
    if (auto brToLoopHeader = llvm::dyn_cast<GotoExpr>( loopPreheaderTerminator)) {
        if ( brToLoopHeader->target != func->getBlock( loop->getHeader() ) ) {
            assert(false && "This loop is not in the canonical: simplified form!");
        }
    }

    // remove `goto loopHeader;`
    loopPreheader->expressions.pop_back();

    auto doWhileTerminatorInst = llvm::dyn_cast<llvm::BranchInst>(
                            loop->getLoopLatch()->getTerminator());

    auto whileCondIsTrueBlock = llvm::dyn_cast<llvm::BasicBlock>(doWhileTerminatorInst->getOperand(2));
    Expr* cmp =  func->getExpr(
                        doWhileTerminatorInst->getCondition()
                            );
    if ( whileCondIsTrueBlock != loop->getHeader() ) {
            auto cmpExpr = llvm::dyn_cast<CmpExpr>( cmp );
            cmpExpr->comparsion = getComparePredicate(negateCmpInst(llvm::dyn_cast<llvm::CmpInst>(doWhileTerminatorInst->getCondition())));

            llvm::errs() << "flipped\n";
    }
    // do { goto loop.header() } while (c)

    // #2 create the doWhile body `do { goto loopHeader; }`
    Block* doWhileBody = func->createBlockIfNotExist( loop->getHeader() );

    // #3 create doWhileBody expr
    auto doWhile = std::make_unique<DoWhile>(createListOfOneGoto( func->getBlock( loop->getLoopLatch() ),
                                                                    doWhileBody, true /*isLoop = true*/),
                                                cmp);

    // #4 it's safe to inline loopheader:
    // loopHeader.succ_first() == loopLatch, loopHeader.succ_second() = loopPreheader
    // we transform the edge from the loopLatch to loopHeader into to do{}while() block
    // there will be single succesor - the edge from loopPreheader leading to the loopHeader
    doWhileBody->doInline = true;

    bool headerTerGOTOlatch = (loop->getHeader()->getTerminator()->getNumSuccessors() == 1 &&
                               loop->getHeader()->getTerminator()->getSuccessor(0) == loop->getLoopLatch());
    //##############################################
    /* size(path[header -> latch]) > 1 */
    if ( (loop->getHeader() != loop->getLoopLatch()) && !headerTerGOTOlatch) {
        auto latchWrap = std::make_unique<LatchExpr> ( func->getBlock( loop->getLoopLatch() ), false);

        llvm::dyn_cast<ExprList>(doWhile->body)->expressions.push_back( latchWrap.get() );
        doWhileBody->addOwnership( std::move(latchWrap) );
    }
    //##############################################
    func->getBlock( loop->getLoopLatch() )->brHandled = true;

    // "dummy force inlining": add DOWHILE to the preheader block
    // (on the place were the goto doWhileHeader should be)
    loopPreheader->addExpr( doWhile.get() );
    func->createExpr( loop->getLoopLatch()->getTerminator(), std::move( doWhile ) );

    
    // when `eval( C ) == false, for C from `while( C )`, we are jumping to the next block after doWhile
    Block* afterDoWhile = func->createBlockIfNotExist(
                                (llvm::dyn_cast<llvm::BasicBlock>(
                                        loop->getLoopLatch()->getTerminator()->getOperand(1)
                                        ) == loop->getHeader() ? llvm::dyn_cast<llvm::BasicBlock>(
                                                                 loop->getLoopLatch()->getTerminator()->getOperand(2)) :
                                                                 llvm::dyn_cast<llvm::BasicBlock>(
                                                                 loop->getLoopLatch()->getTerminator()->getOperand(1)))
                                        );
    // add `goto afterDoWhileBlock;` expression right after the doWhile expr
    //loopPreheader->addExpr(createListOfOneGoto(loopPreheader, afterDoWhile));
    loopPreheader->addExpr(createListOfOneGoto(func->getBlock( loop->getLoopLatch() ), afterDoWhile));

    // nested loops are already parsed
    // parse yet unparsed blocks
    for( const auto& loopBodyBlock : loop->blocks() ) {
        if (!func->getBlock(loopBodyBlock)->brHandled)
            parseBlock(func, loopBodyBlock);
    }
}

void parseBreakRec(const llvm::Module* module, Program& program) {
    assert(program.isPassCompleted(PassType::CreateExpressions));
    for (const auto& function : module->functions()) {
        if(function.isIntrinsic() || function.isDeclaration()) continue;
        auto* func = program.getFunction(&function);

        llvm::LoopInfo &LI = func->FAM.getResult<llvm::LoopAnalysis>(const_cast<llvm::Function &>(*func->function));
        for( const auto& loop : LI)
            parseLoop(func, loop);

        for (const auto& block : function) {
            if (func->getBlock( &block )->brHandled)
                continue;
            parseBlock( func, &block);
        }
    }
}
/*
static void parseCondBranch(const llvm::BranchInst& ins, Func* func, Block* block) {

    llvm::LoopInfo &LI = func->FAM.getResult<llvm::LoopAnalysis>(const_cast<llvm::Function &>(*func->function));
    // llvm::errs() << *LI.begin();
    auto it = std::find_if(LI.begin(), LI.end(), [&ins](llvm::Loop *l){
        return llvm::dyn_cast<llvm::BranchInst>(l->getLoopLatch()->getTerminator())->getCondition() == ins.getCondition();});

    if (  LI.end() != it && (*it)->contains(block->block) ) {
        llvm::errs() << "doWhile created\n";

        if ((*it)->getHeader() == (*it)->getLoopLatch()) { // #1 HEADER and LATCH are same
            llvm::errs() << "LATCH is HEADER\n";
            // move all exprs into the body of doWhile!
            Block* loopNodes = func->createBlockIfNotExist((llvm::BasicBlock*)ins.getOperand(2));
            auto doWhileBody = std::make_unique<ExprList>(std::move( loopNodes->expressions ));
            Expr *doWhileBodyPtr = doWhileBody.get();

            // create condition
            Expr* cmp = func->getExpr(ins.getCondition());

            // create doWhile
            auto doWhile = std::make_unique<DoWhile>(doWhileBodyPtr, cmp);
            loopNodes->addOwnership(std::move(doWhileBody));
            func->createExpr(&ins, std::move(doWhile));
            Block* afterDoWhileBlock = func->createBlockIfNotExist((llvm::BasicBlock*)ins.getOperand(1));
            block->addExpr(func->getExpr(&ins));
            block->addExpr(createListOfOneGoto(block, afterDoWhileBlock));
            block->brHandled = true;
            return;

        }
        if ((*it)->getHeader()->getTerminator()->getSuccessor(0) == (*it)->getLoopLatch()) {// #2 latch --SINGLE-EDGE--> header
            llvm::errs() << "HOIST LATCH TO HEADER\n";

            Expr* cmp = func->getExpr(ins.getCondition());
            Block* HeaderBlock = func->getBlock((*it)->getHeader());

            if (llvm::dyn_cast_or_null<GotoExpr>(HeaderBlock->expressions.back())->target != block)
                assert(false && "The header is not jumping into the (latch) block!");
            HeaderBlock->expressions.pop_back(); // Remove the uncoditional jump into the latch block

            auto doWhileBody = std::make_unique<ExprList>(std::move( HeaderBlock->expressions ));
            
            if( block->expressions.size()) { //If the latch contains any EXPRS add also them inside the doWhile block:
                auto latchNodeBody = std::make_unique<ExprList>(std::move( block->expressions ));
                doWhileBody->expressions.push_back( latchNodeBody.get() );
                HeaderBlock->addOwnership( std::move( latchNodeBody ) );
            }
            
            Expr *doWhileBodyPtr = doWhileBody.get();
            auto doWhile = std::make_unique<DoWhile>(doWhileBodyPtr, cmp);
            HeaderBlock->addOwnership( std::move(doWhileBody) );
            func->createExpr(&ins, std::move(doWhile));
            HeaderBlock->addExpr(func->getExpr(&ins));

            Block* afterDoWhileBlock = func->createBlockIfNotExist((llvm::BasicBlock*)ins.getOperand(1));
            HeaderBlock->addExpr(createListOfOneGoto(HeaderBlock, afterDoWhileBlock));
            block->brHandled = true;
            return; // block is not our daddy, but rather: HeaderBlock->addExprAndOwnership 
            
        }
        Expr* cmp = func->getExpr(ins.getCondition());
        Block* doWhileBody = func->createBlockIfNotExist((llvm::BasicBlock*)ins.getOperand(2));
        auto doWhile = std::make_unique<DoWhile>(createListOfOneGoto(block, doWhileBody), cmp);
        func->createExpr(&ins, std::move(doWhile));
        block->addExpr(func->getExpr(&ins));
        Block* afterDoWhile = func->createBlockIfNotExist((llvm::BasicBlock*)ins.getOperand(1));
        block->addExpr(createListOfOneGoto(block, afterDoWhile));
        return;


        // Loop with complex CFG inside
        // parseComplexLoop(ins, func, block, *it);
        // std::terminate();
        assert(false && "Something unloopected happend!");
    } else {
        llvm::errs() << "Ifcond created\n";
        Expr* cmp = func->getExpr(ins.getCondition());

        Block* falseBlock = func->createBlockIfNotExist((llvm::BasicBlock*)ins.getOperand(1));
        Block* trueBlock = func->createBlockIfNotExist((llvm::BasicBlock*)ins.getOperand(2));

        auto ifExpr = std::make_unique<IfExpr>(cmp, createListOfOneGoto(block, trueBlock), createListOfOneGoto(block, falseBlock));
        func->createExpr(&ins, std::move(ifExpr));
    }

    block->brHandled = true;
    block->addExpr(func->getExpr(&ins));
}
*/
/*
// static void parseBrInstruction(const llvm::BranchInst& ins, Func* func, Block* block) {
//     //no condition
//     if ( ins.isUnconditional() )
//         return parseUncondBranch(ins, func, block);
    
//     parseCondBranch(ins, func, block);
// }

// static void parseRetInstruction(const llvm::Instruction& ins, Func* func, Block* block) {
//     const llvm::Value* value = &ins;

//     if (ins.getNumOperands() == 0) {
//         func->createExpr(value, std::make_unique<RetExpr>());
//     } else {
//         Expr* expr = func->getExpr(ins.getOperand(0));
//         assert(expr);

//         func->createExpr(value, std::make_unique<RetExpr>(expr));
//     }

//     block->brHandled = true;
//     block->addExpr(func->getExpr(&ins));
// }
*/

void parseBreaks(const llvm::Module* module, Program& program) {
    assert(program.isPassCompleted(PassType::CreateExpressions));
    parseBreakRec(module, program);
    

    // for (const auto& function : module->functions()) {
    //     auto* func = program.getFunction(&function);
    //     for (const auto& block : function) {
    //         auto* myBlock = func->getBlock(&block);
    //         for (const auto& ins : block) {
    //             auto opcode = ins.getOpcode();
    //             if (opcode == llvm::Instruction::Br) {
    //                 const llvm::BranchInst* br = llvm::dyn_cast<llvm::BranchInst>(&ins);
    //                 parseBrInstruction(*br, func, myBlock);
    //             } else if (opcode == llvm::Instruction::Ret) {
    //                 parseRetInstruction(ins, func, myBlock);
    //             }
    //         }
    //     }
    // }

    program.addPass(PassType::ParseBreaks);
}
