#include "../core/Program.h"
#include "../core/Func.h"
#include "../core/Block.h"

#include <llvm/IR/Instruction.h>

static void setMetadataInfo(const llvm::CallInst* ins, Block* block) {
    llvm::Metadata* md = llvm::dyn_cast_or_null<llvm::MetadataAsValue>(ins->getOperand(0))->getMetadata();
    llvm::Value* referredVal = llvm::cast<llvm::ValueAsMetadata>(md)->getValue();

    if (Value* variable = static_cast<Value*>(block->func->getExpr(referredVal))) {
        llvm::Metadata* varMD = llvm::dyn_cast_or_null<llvm::MetadataAsValue>(ins->getOperand(1))->getMetadata();
        llvm::DILocalVariable* localVar = llvm::dyn_cast_or_null<llvm::DILocalVariable>(varMD);
        llvm::DIBasicType* type = llvm::dyn_cast_or_null<llvm::DIBasicType>(localVar->getType());

        if (!localVar->getName().empty()) {
            variable->valueName = localVar->getName();
        }

        if (type && type->getName().str().compare(0, 8, "unsigned") == 0) {
            if (IntegerType* IT = llvm::dyn_cast_or_null<IntegerType>(variable->getType())) {
                IT->unsignedType = true;
            }
        }
    }
}

void parseMetadataTypes(const llvm::Module* module, Program& program) {
    assert(program.isPassCompleted(PassType::CreateBlocks));

    for (const auto& function : module->functions()) {
        auto* func = program.getFunction(&function);
        for (const auto& block : function) {
            auto* myBlock = func->getBlock(&block);

            for (const auto& ins : block) {
                if (ins.getOpcode() == llvm::Instruction::Call) {
                    const llvm::CallInst* CI = llvm::cast<llvm::CallInst>(&ins);
                    if (CI->getCalledFunction()) {
                        if (CI->getCalledFunction()->getName().str().compare("llvm.dbg.declare") == 0) {
                            setMetadataInfo(CI, myBlock);
                        }
                    }
                }
            }

        }
    }

    program.addPass(PassType::ParseMetadataTypes);
}
