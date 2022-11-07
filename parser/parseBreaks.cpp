#include "../core/Program.h"
#include "../core/Func.h"
#include "../core/Block.h"
#include "cfunc.h"
#include "constval.h"

#include <llvm/IR/Instruction.h>

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
    Expr* cmp = func->getExpr(ins.getCondition());

    Block* falseBlock = func->createBlockIfNotExist((llvm::BasicBlock*)ins.getOperand(1));
    Block* trueBlock = func->createBlockIfNotExist((llvm::BasicBlock*)ins.getOperand(2));

    auto ifExpr = std::make_unique<IfExpr>(cmp, createListOfOneGoto(block, trueBlock), createListOfOneGoto(block, falseBlock));

    func->createExpr(&ins, std::move(ifExpr));
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
