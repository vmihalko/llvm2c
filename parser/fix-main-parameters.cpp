#include "../core/Program.h"
#include "../core/Func.h"
#include "../core/Block.h"

#include <llvm/IR/Instruction.h>
#include <iostream>

static void convertToSignedIntPtr(PointerType* pt) {
    if (auto inner = llvm::dyn_cast_or_null<PointerType>(pt->type.get())) {
        convertToSignedIntPtr(inner);
    } else if (auto IT = llvm::dyn_cast_or_null<IntegerType>(pt->type.get())) {
        IT->unsignedType = false;
    }
}

void fixMainParameters(const llvm::Module* module, Program& program) {
    for (const llvm::Function& func : module->functions()) {
        auto* myFunc = program.getFunction(&func);
        auto* decl = program.getDeclaration(&func);

        if (func.getName() != "main") {
            continue;
        }

        static_cast<IntegerType*>(myFunc->returnType.get())->unsignedType = false;
        static_cast<IntegerType*>(decl->returnType.get())->unsignedType = false;

        for (auto& param : myFunc->parameters) {
            auto type = param->getType();
            if (auto IT = llvm::dyn_cast_or_null<IntegerType>(type)) {
                IT->unsignedType = false;
            }

            if (auto PT = llvm::dyn_cast_or_null<PointerType>(type)) {
                convertToSignedIntPtr(PT);
            }
        }

        for (auto& param : decl->parameters) {
            auto type = param->getType();
            if (auto IT = llvm::dyn_cast_or_null<IntegerType>(type)) {
                IT->unsignedType = false;
            }

            if (auto PT = llvm::dyn_cast_or_null<PointerType>(type)) {
                convertToSignedIntPtr(PT);
            }
        }
    }
}
