#include "../core/Program.h"
#include "../core/Func.h"
#include "../core/Block.h"
#include "../type/Type.h"

#include <llvm/IR/Instruction.h>
#include <llvm/IR/IntrinsicInst.h>

Type *fixType(Program& program, const llvm::DIType *ditype);

void vectorToString( const std::vector<std::string> &vectorOfStrings, std::string &result) {
    result = "(";
    std::for_each(vectorOfStrings.begin(), vectorOfStrings.end(),
                  [&](const std::string& param) {
                      if (result.size() > 1) {
                          result += ", ";
                      }
                      result += param;
                  });
    result += ")";
}

Type * getFnctnPtrType(Program& program, const llvm::DIDerivedType *diDtype,
                                         const llvm::DISubroutineType *diStype) {

        std::string rtrnType = fixType(program, (*diStype->getTypeArray().begin()))->toString();

        std::vector<std::string> fnctnTypesOfArgs;
        std::transform(++diStype->getTypeArray().begin(), diStype->getTypeArray().end(),
                       std::back_inserter(fnctnTypesOfArgs),
                   [&program](auto argType){ return fixType(program, argType)->toString(); });

        std::string fnctnTypesOfArgsString;
        vectorToString( fnctnTypesOfArgs, fnctnTypesOfArgsString); // e.g. "(int, doubl, char *)"

        auto nthTypeDef = program.typeHandler.getTypeDefNumber();
        program.typeHandler.ditypeCache[diDtype] = std::make_unique<FunctionPointerType>(
                    rtrnType + "(*",  "typeDef_" + std::to_string(nthTypeDef), ")" + fnctnTypesOfArgsString);
        program.typeHandler.sortedTypeDefs[nthTypeDef] = static_cast<FunctionPointerType *>(program.typeHandler.ditypeCache[diDtype].get());
        return program.typeHandler.ditypeCache[diDtype].get();
}
Type *fixType(Program& program, const llvm::DIType *ditype) {
        // pointerTypes are cached:
        auto it = program.typeHandler.ditypeCache.find(ditype);
        if (it != program.typeHandler.ditypeCache.end()) {
            return it->second.get();
        }
        // Int or float
        if (auto tbasic = llvm::dyn_cast_or_null<llvm::DIBasicType>(ditype)) {
            if (!tbasic->getSignedness().hasValue()) {
                if (tbasic->getEncoding() == llvm::dwarf::DW_ATE_float) {
                    if (tbasic->getSizeInBits() <= 32)
                        return program.typeHandler.floatType.get();
                    if (tbasic->getSizeInBits() <= 64)
                        return program.typeHandler.doubleType.get();
                    return program.typeHandler.longDoubleType.get();
                }
            }

            bool signedness = llvm::DIBasicType::Signedness::Signed == *tbasic->getSignedness();
            if (tbasic->getSizeInBits() == 1) {
                return (signedness ? program.typeHandler.sint.get() : program.typeHandler.uint.get());
            }
            if (tbasic->getSizeInBits() <= 8) {
                return (signedness ? program.typeHandler.schar.get() : program.typeHandler.uchar.get());
            }
            if (tbasic->getSizeInBits() <= 16) {
                return (signedness ? program.typeHandler.sshort.get() : program.typeHandler.ushort.get());
            }
            if (tbasic->getSizeInBits() <= 32) {
                return (signedness ? program.typeHandler.sint.get() : program.typeHandler.uint.get());
            }
            if (tbasic->getSizeInBits() <= 64) {
                return (signedness ? program.typeHandler.slong.get() : program.typeHandler.ulong.get());
            }
        }
        // DIDerivedType is used to represent a type that is derived
        // from another type, such as a pointer, or typedef.
        const llvm::DIDerivedType* diDerivedType = llvm::dyn_cast<llvm::DIDerivedType>(ditype);
        if( diDerivedType && (diDerivedType->getTag() == llvm::dwarf::DW_TAG_pointer_type)) {

            if (const llvm::DISubroutineType *diStype = llvm::dyn_cast<llvm::DISubroutineType>( diDerivedType->getBaseType() )) {
                return getFnctnPtrType(program, diDerivedType, diStype);
            }
            auto *innerType = fixType(program, diDerivedType->getBaseType());
            return program.typeHandler.cachedDITypeInserter<PointerType>(diDerivedType, innerType);
        }


        llvm::errs() << "Unknown type <"<< ditype->getTag() << ">! Terminating...\n";
        std::terminate();
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

        if (auto t = fixType(program, localVar->getType()))
            variable->setType(t);
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
                    if (CI->getCalledFunction()) {
                        if (CI->getIntrinsicID() == llvm::Intrinsic::dbg_declare) {
                            setMetadataInfo(program, CI, myBlock);
                        }
                    }
                }
            }
        }
    }
    for (const llvm::GlobalVariable& gvar : module->globals()) {
        // Vector bellow is always holding only current gvar,
        // thus for-cycle bellow always has only one iteration.
        llvm::SmallVector<llvm::DIGlobalVariableExpression *, 1> GVs;
        gvar.getDebugInfo(GVs);
        for (auto *GVE : GVs) {
            llvm::DIVariable *Var = GVE->getVariable();
            if ( program.getGlobalVar( &gvar ) ) {
                program.getGlobalVar( &gvar )->expr->setType(
                    fixType(program, Var->getType()));
            } else
                llvm::errs() << " Global v missing, but debuginfo\n";
        }
    }
    program.addPass(PassType::ParseMetadataTypes);
}
