#include "../core/Program.h"

#include <llvm/IR/Instruction.h>
#include <llvm/IR/CFG.h>

#include <iostream>

static bool isSafeToInline(const llvm::BasicBlock* llvmBlock, const Block* block) {
    if (llvmBlock->getSinglePredecessor()) {
        return true;
    }

    if (block->expressions.size() == 1) {
        const Expr* fst = block->expressions.front();
        return llvm::isa<RetExpr>(fst);
    }

    return false;
}

void identifyInlinableBlocks(const llvm::Module* module, Program& program) {
    assert(program.isPassCompleted(PassType::CreateExpressions));
    assert(program.isPassCompleted(PassType::ParseBreaks));

    for (const llvm::Function& func : module->functions()) {
        auto* function = program.getFunction(&func);
        for (const auto& block : func) {
            auto* myBlock = function->createBlockIfNotExist(&block);
            myBlock->doInline = isSafeToInline(&block, myBlock);
        }

        if (func.begin() == func.end())
            continue;

        // first block is only inlinable if there are no predecessors
        auto& firstBlock = *func.begin();
        auto* myBlock = function->createBlockIfNotExist(&firstBlock);

        myBlock->doInline = false;
        myBlock->isFirst = true;
    }

    program.addPass(PassType::IdentifyInlinableBlocks);
}
