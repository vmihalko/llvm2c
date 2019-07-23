#include <llvm/IR/Instructions.h>

#include "../core/Program.h"
#include "constval.h"

static void createConstantsForOperands(const llvm::Instruction& ins, Program& program) {
    for (const auto& op : ins.operands()) {
        if (auto C = llvm::dyn_cast_or_null<llvm::Constant>(op.get())) {
            if (program.getExpr(op.get()) == nullptr)
                program.addExpr(op.get(), createConstantValue(C, program));
        }
    }
}

void createConstants(const llvm::Module* mod, Program& program) {
    for (const auto& func : mod->functions()) {
        for (const auto& bb : func) {
            for (const auto& ins : bb) {
                createConstantsForOperands(ins, program);
            }
        }
    }
}
