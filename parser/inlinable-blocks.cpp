#include "../core/Program.h"

#include <llvm/IR/Instruction.h>

void identifyInlinableBlocks(const llvm::Module* module, Program& program) {
    for (const llvm::Function& func : module->functions()) {
        auto* function = program.getFunction(&func);
        for (const auto& block : func) {
            auto* myBlock = function->createBlockIfNotExist(&block);
            myBlock->doInline = (block.hasNPredecessors(1));
        }
    }
}
