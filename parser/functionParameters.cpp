#include "../core/Program.h"
#include "../core/Func.h"
#include "../core/Block.h"

#include <llvm/IR/Instruction.h>
#include <iostream>

static void createArgs(Program& program, const llvm::Function* llvmFunc, Func* func) {
    const llvm::Value* lastValue;
    for (const llvm::Value& arg : llvmFunc->args()) {
        lastValue = &arg;
        auto argVal = std::make_unique<Value>(func->getVarName(), program.getType(arg.getType()));

        func->parameters.push_back(argVal.get());
        func->createExpr(lastValue, std::move(argVal));
    }

    auto lastArg = func->exprMap[lastValue].get();
    if (lastArg) {
        func->setVarArg(llvmFunc->isVarArg());
    }
}

void createFunctionParameters(const llvm::Module* module, Program& program) {
    for (const llvm::Function& func : module->functions()) {
        auto* myFunc = program.getFunction(&func);
        auto* decl = program.getDeclaration(&func);

        if (myFunc)
            createArgs(program, &func, myFunc);

        if (decl)
            createArgs(program, &func, decl);

    }

}

