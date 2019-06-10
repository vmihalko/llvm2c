#include "../core/Program.h"
#include "../core/Func.h"
#include "../core/Block.h"
#include "cfunc.h"
#include "constval.h"

#include <llvm/IR/Instruction.h>

static void parseBrInstruction(const llvm::Instruction& ins, bool isConstExpr, const llvm::Value* val, Func* func, Block* block) {
    const llvm::Value* value = isConstExpr ? val : &ins;

    //no condition
    if (ins.getNumOperands() == 1) {
        std::string trueBlock = func->createBlockIfNotExist((llvm::BasicBlock*)ins.getOperand(0))->blockName;
        func->createExpr(value, std::make_unique<IfExpr>(trueBlock));

        if (!isConstExpr) {
            block->addExpr(func->getExpr(&ins));
        }
        return;
    }

    Expr* cmp = func->getExpr(ins.getOperand(0));

    std::string falseBlock = func->createBlockIfNotExist((llvm::BasicBlock*)ins.getOperand(1))->blockName;
    std::string trueBlock = func->createBlockIfNotExist((llvm::BasicBlock*)ins.getOperand(2))->blockName;

    func->createExpr(value, std::make_unique<IfExpr>(cmp, trueBlock, falseBlock));

    if (!isConstExpr) {
        block->addExpr(func->getExpr(&ins));
    }
}

static void parseRetInstruction(const llvm::Instruction& ins, bool isConstExpr, const llvm::Value* val, Func* func, Block* block) {
    const llvm::Value* value = isConstExpr ? val : &ins;

    if (ins.getNumOperands() == 0) {
        func->createExpr(value, std::make_unique<RetExpr>());
    } else {
        if (func->getExpr(ins.getOperand(0)) == nullptr) {
            createConstantValue(ins.getOperand(0), func, block);
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
                    parseBrInstruction(ins, false, nullptr, func, myBlock);
                } else if (opcode == llvm::Instruction::Ret) {
                    parseRetInstruction(ins, false, nullptr, func, myBlock);
                }
            }
        }
    }
}
