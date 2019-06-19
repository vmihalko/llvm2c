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

                    auto theVariable = std::make_unique<Value>(func->getVarName(), func->getType(allocaInst->getAllocatedType()));
                    auto alloc = std::make_unique<StackAlloc>(theVariable.get());

                    myBlock->addExpr(alloc.get());

                    myBlock->insertValue(&ins, std::move(theVariable));
                    myBlock->allocas.push_back(std::move(alloc));
                }
            }

        }
    }
}
