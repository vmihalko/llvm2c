#include "../core/Program.h"
#include "../core/Func.h"
#include "../core/Block.h"
#include "../type/TypeHandler.h"

#include <llvm/IR/Instruction.h>


void parseStructItems(const llvm::Module* module, Program& program) {
    assert(program.isPassCompleted(PassType::ParseStructDeclarations));

    for (llvm::StructType* structType : module->getIdentifiedStructTypes()) {
        auto structExpr = llvm::dyn_cast<StructType>(program.typeHandler.getType(structType));
        assert(structExpr && "parseStructItems: corresponding struct declaration not found");

        for (llvm::Type* type : structType->elements()) {
            auto ty = program.getType(type);
            assert(ty && "parseStructItems: corresponding type not found");
            structExpr->addItem(ty, program.getStructVarName());
        }

    }

    program.addPass(PassType::ParseStructItems);
}
