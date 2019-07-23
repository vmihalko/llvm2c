#include <llvm/IR/Instructions.h>

#include "../core/Program.h"
#include "constval.h"

static void createConstantsForOperands(const llvm::User* ins, Program& program) {
    for (const auto& op : ins->operands()) {
        if (auto *user = llvm::dyn_cast_or_null<llvm::User>(op.get())) {
            createConstantsForOperands(user, program);
        }

        if (auto C = llvm::dyn_cast_or_null<llvm::Constant>(op.get())) {
            if (program.getExpr(op.get()) == nullptr)
                program.addExpr(op.get(), createConstantValue(C, program));
        }

    }
}

void createConstants(const llvm::Module* mod, Program& program) {
    for (const auto& gvar : mod->globals()) {
        if (gvar.hasInitializer()) {
            auto* init = gvar.getInitializer();
            if (auto *user = llvm::dyn_cast_or_null<llvm::User>(init)) {
                createConstantsForOperands(user, program);
            }
        }
    }

    for (const auto& func : mod->functions()) {
        for (const auto& bb : func) {
            for (const auto& ins : bb) {
                createConstantsForOperands(&ins, program);
            }
        }
    }
}
