#include "../core/Program.h"
#include "../core/Func.h"
#include "../core/Block.h"

#include <llvm/IR/Instruction.h>
#include <iostream>

static void convertToSignedIntPtr(Program& program, PointerType* pt) {
    if (auto inner = llvm::dyn_cast_or_null<PointerType>(pt->type)) {
        convertToSignedIntPtr(program, inner);
    } else if (auto IT = llvm::dyn_cast_or_null<IntType>(pt->type)) {
        pt->type = program.typeHandler.sint.get();
    } else if (auto CT = llvm::dyn_cast_or_null<CharType>(pt->type)) {
        pt->type = program.typeHandler.schar.get();
    }
}

void fixMainParameters(const llvm::Module* module, Program& program) {
    assert(program.isPassCompleted(PassType::CreateFunctionParameters));

    for (const llvm::Function& func : module->functions()) {
        auto* myFunc = program.getFunction(&func);

        if (func.getName() != "main") {
            continue;
        }

        myFunc->returnType = program.typeHandler.sint.get();

        for (auto& param : myFunc->parameters) {
            auto type = param->getType();
            param->setType(program.typeHandler.sint.get());

            if (auto PT = llvm::dyn_cast_or_null<PointerType>(type)) {
                convertToSignedIntPtr(program, PT);
            }
        }
    }

    program.addPass(PassType::FixMainParameters);
}
