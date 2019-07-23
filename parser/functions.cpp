#include "../core/Program.h"
#include "../core/Func.h"
#include "../core/Block.h"

#include <llvm/IR/Instruction.h>

void createFunctions(const llvm::Module* module, Program& program) {
    assert(program.isPassCompleted(PassType::FindDeclaredFunctions));

    for(const llvm::Function& func : module->functions()) {
        if (func.hasName() && !func.isDeclaration()) {
            auto *function = program.getFunction(&func);
            function->isDeclaration = false;
        }
    }

    program.addPass(PassType::CreateFunctions);
}
