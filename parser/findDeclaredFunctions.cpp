#include <llvm/IR/Instruction.h>

#include "../core/Program.h"
#include "../core/Func.h"
#include "../core/Block.h"

#include <iostream>

static void declareFunc(const llvm::Function* func, Program& program) {
    auto decl = std::make_unique<Func>(func, &program, func->isDeclaration());
    for (const llvm::Value& arg : func->args()) {
        auto argVal = std::make_unique<Value>(decl->getVarName(), program.getType(arg.getType()));
        decl->createExpr(&arg, std::move(argVal));
    }

    if (func->args().begin() != func->args().end()) {
        decl->setVarArg(func->isVarArg());
    }

    // FIXME: a hack for CPAchecker -- do this optional
    if (func->getName().startswith("__VERIFIER_nondet_") &&
        (func->getName().endswith("_int") || 
         func->getName().endswith("_long") || 
         func->getName().endswith("_short") || 
         func->getName().endswith("_char") || 
         func->getName().endswith("_float") ||
         func->getName().endswith("_double"))) {
        decl->returnType = program.typeHandler.setSigned(static_cast<IntegerType*>(decl->returnType));
    }
    program.addFunction(func, std::move(decl));
}

void findDeclaredFunctions(const llvm::Module *module, Program& program) {
    for(const llvm::Function& func : module->functions()) {
        if (func.hasName()) {
            if (func.isDeclaration() || llvm::Function::isInternalLinkage(func.getLinkage())) {
                if (func.getName().str().substr(0, 8) != "llvm.dbg" && func.getName() != "isnan") {
                    if (!program.isFunctionDeclared(&func)) {
                        declareFunc(&func, program);
                    }
                }
            }

            if (!func.isDeclaration()) {
                if (!program.isFunctionDeclared(&func)) {
                    declareFunc(&func, program);
                }
            }
        }
    }

    program.addPass(PassType::FindDeclaredFunctions);
}
