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
            return; // block is not our daddy, but rather: HeaderBlock->addExprAndOwnership 
            
        }
        std::terminate();
        assert(false && "Something unloopected happend!");
    } else {
        llvm::errs() << "Ifcond created\n";
        Expr* cmp = func->getExpr(ins.getCondition());

        Block* falseBlock = func->createBlockIfNotExist((llvm::BasicBlock*)ins.getOperand(1));
        Block* trueBlock = func->createBlockIfNotExist((llvm::BasicBlock*)ins.getOperand(2));

        auto ifExpr = std::make_unique<IfExpr>(cmp, createListOfOneGoto(block, trueBlock), createListOfOneGoto(block, falseBlock));
        func->createExpr(&ins, std::move(ifExpr));
    }

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

    block->addExpr(func->getExpr(&ins));
}

void parseBreaks(const llvm::Module* module, Program& program) {
    assert(program.isPassCompleted(PassType::CreateExpressions));

    for (const auto& function : module->functions()) {
        auto* func = program.getFunction(&function);
        for (const auto& block : function) {
            auto* myBlock = func->getBlock(&block);
            for (const auto& ins : block) {
                auto opcode = ins.getOpcode();
                if (opcode == llvm::Instruction::Br) {
                    const llvm::BranchInst* br = llvm::dyn_cast<llvm::BranchInst>(&ins);
                    parseBrInstruction(*br, func, myBlock);
                } else if (opcode == llvm::Instruction::Ret) {
                    parseRetInstruction(ins, func, myBlock);
                }
            }
        }
    }

    program.addPass(PassType::ParseBreaks);
}
