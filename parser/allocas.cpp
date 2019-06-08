#include <llvm/IR/Instruction.h>
#include "../core/Program.h"
#include "../core/Func.h"
#include "../core/Block.h"

void createAllocas(const llvm::Module* module, Program& program) {
    for (const auto& function : module->functions()) {
        auto* func = program.getFunction(&function);
        for (const auto& block : function) {
            auto* myBlock = func->getBlock(&block);

            for (const auto& ins : block) {
                if (ins.getOpcode() == llvm::Instruction::Alloca) {

                    const auto allocaInst = llvm::cast<const llvm::AllocaInst>(&ins);

                    auto myExpr = std::make_unique<Value>(func->getVarName(), func->getType(allocaInst->getAllocatedType()));

                    myBlock->addExpr(myExpr.get());

                    myBlock->insertValue(&ins, std::move(myExpr));
                }
            }

        }
    }
}
