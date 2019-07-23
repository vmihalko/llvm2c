#include "../core/Program.h"
#include "../core/Func.h"
#include "../core/Block.h"
#include "../type/TypeHandler.h"

#include <llvm/IR/Instruction.h>

static std::unique_ptr<Struct> createVarargStruct(std::string structName) {
    auto structExpr = std::make_unique<Struct>(structName);
    structExpr->addItem(std::make_unique<IntType>(true), "gp_offset");
    structExpr->addItem(std::make_unique<IntType>(true), "fp_offset");
    structExpr->addItem(std::make_unique<PointerType>(std::make_unique<VoidType>()), "overflow_arg_area");
    structExpr->addItem(std::make_unique<PointerType>(std::make_unique<VoidType>()), "reg_save_area");
    return structExpr;
}

void parseStructs(const llvm::Module* module, Program& program) {
    for (llvm::StructType* structType : module->getIdentifiedStructTypes()) {
        std::string structName = TypeHandler::getStructName(structType->getName().str());

        if (structName.compare("__va_list_tag") == 0) {
            program.hasVarArg = true;
            auto structExpr = createVarargStruct(structName);
            program.addStruct(std::move(structExpr));
            continue;
        }

        auto structExpr = std::make_unique<Struct>(structName);

        for (llvm::Type* type : structType->elements()) {
            structExpr->addItem(program.getType(type), program.getStructVarName());
        }

        program.addStruct(std::move(structExpr));
    }

    program.addPass(PassType::ParseStructs);
}
