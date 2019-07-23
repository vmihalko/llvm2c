#include "../core/Program.h"
#include "../core/Func.h"
#include "../core/Block.h"
#include "cfunc.h"
#include "constval.h"

#include <llvm/IR/Instruction.h>

static void parseBrInstruction(const llvm::Instruction& ins, Func* func, Block* block) {
    //no condition
    if (ins.getNumOperands() == 1) {
        Block* trueBlock = func->createBlockIfNotExist((llvm::BasicBlock*)ins.getOperand(0));
        auto ifExpr = std::make_unique<IfExpr>(trueBlock);
        func->program->exprMap[&ins] = ifExpr.get();
        block->addExpr(ifExpr.get());
        func->program->addOwnership(std::move(ifExpr));
        return;
    }

    Expr* cmp = func->getExpr(ins.getOperand(0));

    Block* falseBlock = func->createBlockIfNotExist((llvm::BasicBlock*)ins.getOperand(1));
    Block* trueBlock = func->createBlockIfNotExist((llvm::BasicBlock*)ins.getOperand(2));

    auto ifExpr = std::make_unique<IfExpr>(cmp, trueBlock, falseBlock);
    func->createExpr(&ins, std::move(ifExpr));
    block->addExpr(func->getExpr(&ins));
}

static void parseRetInstruction(const llvm::Instruction& ins, Func* func, Block* block) {
    const llvm::Value* value = &ins;

    if (ins.getNumOperands() == 0) {
        func->createExpr(value, std::make_unique<RetExpr>());
    } else {
        if (func->getExpr(ins.getOperand(0)) == nullptr) {
            createConstantValue(ins.getOperand(0), *func->program);
        }
        Expr* expr = func->getExpr(ins.getOperand(0));

        func->createExpr(value, std::make_unique<RetExpr>(expr));
    }

    block->addExpr(func->getExpr(&ins));
}


void parseBreaks(const llvm::Module* module, Program& program) {
    for (const auto& function : module->functions()) {
        auto* func = program.getFunction(&function);
        for (const auto& block : function) {
            auto* myBlock = func->getBlock(&block);

            for (const auto& ins : block) {
                auto opcode = ins.getOpcode();
                if (opcode == llvm::Instruction::Br) {
                    parseBrInstruction(ins, func, myBlock);
                } else if (opcode == llvm::Instruction::Ret) {
                    parseRetInstruction(ins, func, myBlock);
                }
            }
        }
    }
}
