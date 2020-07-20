#include "toinst.h"

std::unique_ptr<llvm::Instruction, std::function<void(llvm::Instruction*)>> toInst(
    llvm::ConstantExpr* ce){
    return std::unique_ptr<llvm::Instruction, std::function<void(llvm::Instruction*)>>(
        ce->getAsInstruction(),
        [](llvm::Instruction* inst) { inst->deleteValue(); }
    );
}
