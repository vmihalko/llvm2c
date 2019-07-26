#include "../core/Program.h"

#include <llvm/IR/Instructions.h>

#include <algorithm>

void inlineBlocks(const llvm::Module* module, Program& program) {
    assert(program.isPassCompleted(PassType::IdentifyInlinableBlocks));

    for (const llvm::Function& func : module->functions()) {
        auto* function = program.getFunction(&func);
        for (const auto& block : func) {
            auto* myBlock = function->createBlockIfNotExist(&block);
            auto* lastExpr = myBlock->expressions.back();
            if (auto* gotoExpr = llvm::dyn_cast_or_null<GotoExpr>(lastExpr)) {
                Block* target = gotoExpr->target;
                if (target->doInline) {
                    myBlock->expressions.erase(myBlock->expressions.end() - 1);
                    std::copy(target->expressions.begin(), target->expressions.end(), std::back_inserter(myBlock->expressions));
                }
            }
        }

        if (func.begin() == func.end())
            continue;

    }

    program.addPass(PassType::InlineBlocks);
}
