#include "../core/Program.h"
#include "../core/Func.h"
#include "../core/Block.h"

#include <llvm/IR/Instruction.h>
#include <iostream>

static void createArgs(Program& program, const llvm::Function* llvmFunc, Func* func) {
    const llvm::Value* lastValue;
    for (const llvm::Value& arg : llvmFunc->args()) {
        lastValue = &arg;
        auto* argExpr = static_cast<Value*>(func->getExpr(lastValue));

        if (!argExpr) {
            auto argVal = std::make_unique<Value>(func->getVarName(), program.getType(arg.getType()));
            argExpr = argVal.get();
            func->createExpr(lastValue, std::move(argVal));
        }

        func->parameters.push_back(argExpr);
    }

    auto lastArg = program.exprMap[lastValue];
    if (lastArg) {
        func->setVarArg(llvmFunc->isVarArg());
    }
}

void createFunctionParameters(const llvm::Module* module, Program& program) {
    assert(program.isPassCompleted(PassType::CreateFunctions));

    for (const llvm::Function& func : module->functions()) {
        auto* myFunc = program.getFunction(&func);


        if (myFunc)
            createArgs(program, &func, myFunc);

    }

    program.addPass(PassType::CreateFunctionParameters);
}

