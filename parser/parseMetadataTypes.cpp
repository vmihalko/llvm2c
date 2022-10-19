#include "../core/Program.h"
#include "../core/Func.h"
#include "../core/Block.h"

#include <llvm/IR/Instruction.h>
#include <llvm/IR/IntrinsicInst.h>

static Type *fixType(Program& program, const llvm::DIType *ditype, Type *mytype){
        // composite, derived, else
        if( const llvm::DIBasicType* diBasicType = llvm::dyn_cast<llvm::DIBasicType>(ditype)) {
            switch (*diBasicType->getSignedness()) // OPTIONAl -> handle it
            {
            case llvm::DIBasicType::Signedness::Unsigned:
                if (IntegerType* IT = llvm::dyn_cast_or_null<IntegerType>(mytype)) {
                    return program.typeHandler.setUnsigned(IT);
                }
                break;
            case llvm::DIBasicType::Signedness::Signed:
            if (IntegerType* IT = llvm::dyn_cast_or_null<IntegerType>(mytype)) {
                    return program.typeHandler.setSigned(IT);
                }
                break;
            default:
                break;
            }
        } else if( const llvm::DIDerivedType* diDerivedType = llvm::dyn_cast<llvm::DIDerivedType>(ditype)) {
            return fixType(program, diDerivedType->getBaseType(), mytype);
        }
        return nullptr;
}

static void setMetadataInfo(Program& program, const llvm::CallInst* ins, Block* block) {
    llvm::Metadata* md = llvm::dyn_cast_or_null<llvm::MetadataAsValue>(ins->getOperand(0))->getMetadata();
    llvm::Value* referredVal = llvm::cast<llvm::ValueAsMetadata>(md)->getValue();
    Expr* referred = block->func->getExpr(referredVal);

    if (auto* re = llvm::dyn_cast_or_null<RefExpr>(referred)) {
        referred = re->expr;
    }

    if (Value* variable = llvm::dyn_cast_or_null<Value>(referred)) {
        llvm::Metadata* varMD = llvm::dyn_cast_or_null<llvm::MetadataAsValue>(ins->getOperand(1))->getMetadata();
        llvm::DILocalVariable* localVar = llvm::dyn_cast_or_null<llvm::DILocalVariable>(varMD);

        if (auto t = fixType(program, localVar->getType(),variable->getType()))
            variable->setType(t);

        //  && *type->getSignedness() == llvm::DIBasicType::Signedness::Unsigned ) {
        //     if (IntegerType* IT = llvm::dyn_cast_or_null<IntegerType>(variable->getType())) {
        //         //variable->setType(program.typeHandler.setUnsigned(IT));
        //     }
  //      } 
        // if (type && type->getName().str().compare(0, 8, "unsigned") == 0) {
        //}
    }
}

void parseMetadataTypes(const llvm::Module* module, Program& program) {
    assert(program.isPassCompleted(PassType::CreateAllocas));

    for (const auto& function : module->functions()) {
        auto* func = program.getFunction(&function);
        for (const auto& block : function) {
            auto* myBlock = func->getBlock(&block);

            for (const auto& ins : block) {
                if (ins.getOpcode() == llvm::Instruction::Call) {
                    const llvm::CallInst* CI = llvm::cast<llvm::CallInst>(&ins);
                    //CI->getIntrinsicID() ===
                    if (CI->getCalledFunction()) {
                        if (CI->getIntrinsicID() == llvm::Intrinsic::dbg_declare) {
                            setMetadataInfo(program, CI, myBlock);
                        }
                    }
                }
            }

        }
    }

    program.addPass(PassType::ParseMetadataTypes);
}
