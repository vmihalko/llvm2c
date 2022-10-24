#include "../core/Program.h"
#include "../core/Block.h"
#include "../expr/Expr.h"

#include <llvm/IR/Instruction.h>

static void deleteExprFromBlock(Block* block, Expr* toDelete) {
    for (auto it = block->expressions.begin(); it != block->expressions.end(); ++it) {
        if (*it == toDelete) {
            block->expressions.erase(it);
            return;
        }
    }
}

// this pass extracts variables from blocks into function beginning
void extractVars(const llvm::Module* module, Program& program) {
    assert(program.isPassCompleted(PassType::CreateAllocas));
    assert(program.isPassCompleted(PassType::CreateExpressions));

    for (const auto& func : module->functions()) {
        auto* function = program.getFunction(&func);

        for (const auto& block : func) {
            std::vector<StackAlloc*> allocs;
            // 1. find allocas
            auto* myBlock = function->getBlock(&block);
            for (const auto& expr : myBlock->expressions) {
                if (auto alloc = llvm::dyn_cast_or_null<StackAlloc>(expr)) {
                    allocs.push_back(alloc);
                }
            }

            for (auto* alloc : allocs) {
                // 2. remove them from block->expressions (ownership can stay in principle)
                deleteExprFromBlock(myBlock, alloc);

                // 3. add them at the beginning of function
                function->variables.push_back(alloc->value);
            }

        }

    }

    program.addPass(PassType::ExtractVars);
}
