#include "../core/Program.h"
#include "../core/Func.h"
#include "../core/Block.h"
#include "../type/Type.h"

#include "constval.h"

#include <llvm/IR/Instruction.h>

#include <regex>
#include <unordered_set>

void initializeGlobalVars(const llvm::Module* module, Program& program) {
    assert(program.isPassCompleted(PassType::CreateConstants));
    assert(program.isPassCompleted(PassType::ComputeGlobalVarsOrder));

    for (const llvm::GlobalVariable& gvar : module->globals()) {
        if (llvm::isa<llvm::Function>(&gvar)) {
            continue;
        }

        if (gvar.hasInitializer()) {
            auto* ref = program.getGlobalVar(&gvar);
            auto* var = llvm::cast<GlobalValue>(ref->expr);
            var->value = createConstantValue(gvar.getInitializer(), program);
        }
    }

    program.addPass(PassType::InitializeGlobalVars);
}
