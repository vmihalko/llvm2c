#include "../core/Program.h"
#include "../core/Func.h"
#include "../core/Block.h"

#include <llvm/IR/Instruction.h>

void createFunctions(const llvm::Module* module, Program& program) {
    for(const llvm::Function& func : module->functions()) {
        if (func.hasName() && !func.isDeclaration()) {
            auto newFun = std::make_unique<Func>(&func, &program, false);
            program.addFunction(&func, std::move(newFun));
        }
    }
}
