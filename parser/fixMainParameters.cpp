#include "../core/Program.h"
#include "../core/Func.h"
#include "../core/Block.h"

#include <llvm/IR/Instruction.h>
#include <iostream>

void fixMainParameters(const llvm::Module* module, Program& program) {
    assert(program.isPassCompleted(PassType::CreateFunctionParameters));

    auto* uchar_ptr_ptr = program.typeHandler.pointerTo(program.typeHandler.pointerTo(program.typeHandler.schar.get()));

    for (const llvm::Function& func : module->functions()) {
        if (func.getName() != "main") {
            continue;
        }

        auto* myFunc = program.getFunction(&func);

        if (func.getReturnType()->isIntegerTy())
            myFunc->returnType = program.typeHandler.sint.get();

        if (myFunc->parameters.size() >= 1) {
            myFunc->parameters[0]->setType(program.typeHandler.sint.get());
            myFunc->parameters[0]->setName("argc");
        }

        if (myFunc->parameters.size() >= 2) {
            myFunc->parameters[1]->setType(uchar_ptr_ptr);
            myFunc->parameters[1]->setName("argv");
        }

    }

    program.addPass(PassType::FixMainParameters);
}
