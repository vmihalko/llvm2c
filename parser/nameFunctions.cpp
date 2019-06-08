#include "cfunc.h"

#include "../core/Program.h"

#include <llvm/IR/Instruction.h>

static void nameFunction(const llvm::Function* llvmFunc, Func* func) {
    auto name = llvmFunc->getName().str();
	if (isCFunc(trimPrefix(name))) {
		name = trimPrefix(name);
		if (name.compare("va_start") == 0
			|| name.compare("va_end") == 0
			|| name.compare("va_copy") == 0
			|| isCMath(name)) {
		}
	}

	if (name.substr(0, 4).compare("llvm") == 0) {
		std::replace(name.begin(), name.end(), '.', '_');
	}

    func->name = name;
}

void nameFunctions(const llvm::Module* module, Program& program) {
    for (const llvm::Function& func : module->functions()) {
        auto* function = program.getFunction(&func);
        auto *decl = program.getDeclaration(&func);

        if (function)
            nameFunction(&func, function);

        if (decl)
            nameFunction(&func, decl);
    }
}
