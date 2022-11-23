#include "../core/Program.h"
#include "../core/Func.h"
#include "../core/Block.h"
#include "cfunc.h"
#include "constval.h"

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

static Expr* createListOfOneGoto(Block* container, Block* gotoTarget) {
    auto gotoExpr = std::make_unique<GotoExpr>(gotoTarget);

    std::vector<Expr*> exprs = generatePhiAssignments(container, gotoTarget);
    exprs.push_back(gotoExpr.get());
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

void parseComplexLoop(const llvm::BranchInst& ins, Func* func, Block* block, llvm::Loop *loop) {
    // This is the doWhile condition
    Expr* doWhileCondExpr = func->getExpr(ins.getCondition());

    Block* HeaderBlock = func->getBlock( loop->getHeader() );
    HeaderBlock->expressions;


    //create doWhile node
    Block* loopNodes = func->createBlockIfNotExist((llvm::BasicBlock*)ins.getOperand(2));
    auto doWhileBody = std::make_unique<ExprList>(std::move( loopNodes->expressions ));
    Expr *doWhileBodyPtr = doWhileBody.get();
    // merge eveything into doWhile body
    return;
}

Expr *parseRETURN(const llvm::ReturnInst& retIns, Func* func/*, Block *block*/) {
    if ( !retIns.getReturnValue() ) {
        func->createExpr(&retIns, std::make_unique<RetExpr>());
    } else {
        func->createExpr(&retIns, std::make_unique<RetExpr>(func->getExpr(retIns.getReturnValue())));
    }
    return func->getExpr(&retIns);
}
Expr *parseIF(const llvm::BranchInst& ins, Func* func, Block* block) {
        llvm::errs() << "Ifcond created\n";
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
    llvm::errs() << "Parsing: " << currBlock->blockName << "\n";

    Expr *terExpr;
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
    llvm::errs() << currBlock->blockName << " parsed: " << "\n";

    return terExpr;
}

void parseLoop(Func* func, const llvm::Loop *loop) {
    for( const auto& subLoop : loop->getSubLoops()) {
        parseLoop( func, subLoop );
    }
        // Do we need to differantiate between loops not containnig any loops
    // and loop wich already processed all subLoops?

    // ------------------------------------------------------------------------------------

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
        if ( brToLoopHeader->target != func->getBlock( loop->getHeader() ) )
            assert(false && "This loop is not in the canonical: simplified form!");
        llvm::errs() << brToLoopHeader->target->blockName << " != " << func->getBlock( loop->getHeader() )->blockName << "\n";    
        llvm::errs() << brToLoopHeader->target << " != " << func->getBlock( loop->getHeader() ) << "\n";    
    }
    // remove `goto loopHeader;`
    loopPreheader->expressions.pop_back();

    // #1 create the `do` and `goto loopBody;` `do { goto loopHeader; }`
    Block* doBody = func->createBlockIfNotExist( loop->getHeader() );
    doBody->doInline = true; //TODO check if there is no more than 2 precessors

    // #3 create doWhileBody expr
    // auto doWhile = std::make_unique<DoWhile>(createListOfOneGoto( func->getBlock( loop->getLoopLatch() ),
    //                                                                 doWhileBody),
    //                                             cmp);
    auto _do = std::make_unique<Do>(createListOfOneGoto( loopPreheader, doBody ));
    loopPreheader->addExprAndOwnership(std::move(_do));
    
    // we run into a problem, 
    // Must every Expr be in a function (ownership)?
    //func->createExpr( *_d, std::move( _do ));


    // #1 create the condition c from `while( C )`
    Expr* cmp = func->getExpr( loop->getLatchCmpInst() );
    auto _while = std::make_unique<While>( nullptr, cmp );
    func->createExpr( loop->getLatchCmpInst(), std::move( _while ));
    func->getBlock( loop->getLoopLatch() )->brHandled = true;
    func->getBlock( loop->getLoopLatch() )->addExpr( func->getExpr(loop->getLatchCmpInst()));
    Block* afterDoWhile = func->createBlockIfNotExist(
                                llvm::dyn_cast<llvm::BasicBlock>(
                                        loop->getLoopLatch()->getTerminator()->getOperand(1)
                                        ));
    func->getBlock( loop->getLoopLatch() )->addExpr(createListOfOneGoto(func->getBlock( loop->getLoopLatch() ), afterDoWhile));

    // add `goto afterDoWhileBlock;` expression after doWhile expr
    // func->getBlock( loop->getLoopLatch() )
    // do { goto loop.header() } while (c)

    // mark latchNode as processed
    
    // TODO: expression in latchnode! 

    // save the newly created expression
    
    // intuition behind this step can be described as "dummy force inlining"
    // add it to the preheader block (on the place were )
    // loopPreheader->expressions.push_back( func->getExpr( loop->getLatchCmpInst() ) );

    // auto latchBlock = func->getBlock( loop->getLoopLatch() ); // in this block loop ~= latch node ~= while ( c ) i=
    // latchBlock->addExpr( func->getExpr( loop->getLatchCmpInst() )); // while condition == DoWhile
    
    // when `eval( C ) == false, for C from `while( C )`, we are jumping to the next block after doWhile

    // nested loops are already parsed
    for( const auto& loopBodyBlock : loop->blocks() ) {
        if (!func->getBlock(loopBodyBlock)->brHandled)
            parseBlock(func, loopBodyBlock);
    }

    // // not containing any loops
    // for( const auto& loopBodyBlock : loop->blocks() ) {
    //     parseBlock(func, loopBodyBlock);
    // }
}

void parseBreakRec(const llvm::Module* module, Program& program) {
    assert(program.isPassCompleted(PassType::CreateExpressions));
    for (const auto& function : module->functions()) {
        if(function.isIntrinsic()) continue;
        auto* func = program.getFunction(&function);
        llvm::LoopInfo &LI = func->FAM.getResult<llvm::LoopAnalysis>(const_cast<llvm::Function &>(*func->function));
        for( const auto& loop : LI) {
            parseLoop(func, loop);
            for (const auto& block : function) {
                if (func->getBlock( &block )->brHandled)
                    continue;
                parseBlock( func, &block);
            }
        //     for( const auto& it = function.begin(); !it.isEnd() &&
        //                                             !func->getBlock( &(*it) )->brHandled;
        //                                             std::next(it) )
                
        //         parseBlock( func, &(*it) );
        // }
    }

    // for (const auto& function : module->functions()) {
    //     auto* func = program.getFunction(&function);
    //     for (const auto& block : function) {
    //         auto* myBlock = func->getBlock(&block);
    //         parseBlock(func, &block);
    //     }
    // }
}
}

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

static void parseBrInstruction(const llvm::BranchInst& ins, Func* func, Block* block) {
    //no condition
    if ( ins.isUnconditional() )
        return parseUncondBranch(ins, func, block);
    
    parseCondBranch(ins, func, block);
}

static void parseRetInstruction(const llvm::Instruction& ins, Func* func, Block* block) {
    const llvm::Value* value = &ins;

    if (ins.getNumOperands() == 0) {
        func->createExpr(value, std::make_unique<RetExpr>());
    } else {
        Expr* expr = func->getExpr(ins.getOperand(0));
        assert(expr);

        func->createExpr(value, std::make_unique<RetExpr>(expr));
    }

    block->brHandled = true;
    block->addExpr(func->getExpr(&ins));
}

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
