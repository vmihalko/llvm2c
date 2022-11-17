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
// TODO?
// We do not want to traverse through all loops every time.. O(n^2), where n i number of loops.
// Find way how to uniquely map branching instruction with concrete loops. (nested loops might be problem)

// static llvm::Loop *inInstructionOutLoop(const llvm::BranchInst& ins, Func* func, Block* block, llvm::LoopInfo &LI) {
//     for (const auto &loop : LI){
//         // loop->getLatchCmpInst
//     }
// }
static void parseCondBranch(const llvm::BranchInst& ins, Func* func, Block* block) {

    // if (std::end(loops) != std::find(loops->begin(), loops->end(), ins.getCondition() )) {

    // }

    llvm::LoopInfo &LI = func->FAM.getResult<llvm::LoopAnalysis>(const_cast<llvm::Function &>(*func->function));
    auto it = std::find_if(LI.begin(), LI.end(), [&ins](llvm::Loop *l){
        return llvm::dyn_cast<llvm::BranchInst>(l->getLoopLatch()->getTerminator())->getCondition() == ins.getCondition();});

    if (  LI.end() != it && (*it)->contains(block->block) ) {
        llvm::errs() << "doWhile created\n";
        // 3 options
        // #1 latch == header
        if ((*it)->getHeader() == (*it)->getLoopLatch() /*|| HEAD ---1---> LATCH */) {
            Expr* cmp = func->getExpr(ins.getCondition());
            Block* loopNodes = func->createBlockIfNotExist((llvm::BasicBlock*)ins.getOperand(2));
            for (auto e : loopNodes->expressions)
                llvm::errs() << "Kind: " << e->getKind() << "\n";

            // I want to move all expresions into doWhile!
            auto doWhileBody = std::make_unique<ExprList>(std::move( loopNodes->expressions ));
            Expr *doWhileBodyPtr = doWhileBody.get();
            auto doWhile = std::make_unique<DoWhile>(doWhileBodyPtr, cmp);
            loopNodes->addOwnership(std::move(doWhileBody));
            func->createExpr(&ins, std::move(doWhile));
            // body of our doWhile should not contain any goto.
        }
        // llvm::errs() << *(*it)->getHeader()->getTerminator()->getSuccessor(0);
        // #2 latch --EDGE--> header
        if ((*it)->getHeader()->getTerminator()->getSuccessor(0) == (*it)->getLoopLatch()) {
            // HOIST LATCH TO HEADER
            // Why not to use llvm functions?
            // because in this moment block were already parsed...
            Expr* cmp = func->getExpr(ins.getCondition());
            Block* loopNodes = func->createBlockIfNotExist((llvm::BasicBlock*)ins.getOperand(2));
            for (auto e : loopNodes->expressions)
                llvm::errs() << "Kind: " << e->getKind() << "\n";

            // I want to move all expresions into doWhile!
            auto doWhileBody = std::make_unique<ExprList>(std::move( loopNodes->expressions ));
            Expr *doWhileBodyPtr = doWhileBody.get();
            auto doWhile = std::make_unique<DoWhile>(doWhileBodyPtr, cmp);
            loopNodes->addOwnership(std::move(doWhileBody));
            func->createExpr(&ins, std::move(doWhile));
        }
        // llvm::errs << *ins;
        // ((llvm::BasicBlock*)ins.getOperand(2))->print(llvm::errs());
        // loopNodes
        // create block with loopNodes
        // create at least something for now
        
        // header ---1---> latch
        // header ---*---> latch
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
