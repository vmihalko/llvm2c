    #include "../core/Program.h"
#include "../core/Func.h"
#include "../core/Block.h"
#include "../type/Type.h"

#include <llvm/IR/Instruction.h>
#include <llvm/IR/IntrinsicInst.h>


template <typename T>
void p(T arg) {
  llvm::errs() << arg << " ";
}

template<typename T, typename... Args>
void p(T t, Args... toPrint) {
    llvm::errs() << t << " ";
    p( toPrint... );
    llvm::errs() << "\n";
}

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

        // std::string rtrnType = (*diStype->getTypeArray().begin() != NULL ? 
        //                                         fixType(program, (*diStype->getTypeArray().begin())
        //                                             )->toString() :
        //                                             "void");
        std::string rtrnType = fixType(program, (*diStype->getTypeArray().begin()))->toString();
        
        std::vector<std::string> fnctnTypesOfArgs;
        std::transform(diStype->getTypeArray().begin()++, diStype->getTypeArray().end(), 
                       std::back_inserter(fnctnTypesOfArgs),
                   [&program](auto argType){ return fixType(program, argType)->toString(); });

        // for (auto argType : diStype->getTypeArray()) {
        //         if ( argType )
        //             fnctnTypesOfArgs.emplace_back(
        //                 fixType(program, argType)->toString()
        //             );
        //         else
        //             fnctnTypesOfArgs.emplace_back("void");
        // }
        std::string fnctnTypesOfArgsString;
        vectorToString( fnctnTypesOfArgs, fnctnTypesOfArgsString); // e.g. "(int, doubl, char *)"

        program.typeHandler.ditypeCache[diDtype] = std::make_unique<FunctionPointerType>(
                    rtrnType + "(*", program.typeHandler.getTypeDefName(), ")" + fnctnTypesOfArgsString);
        program.typeHandler.sortedTypeDefs.push_back(static_cast<FunctionPointerType *>(program.typeHandler.ditypeCache[diDtype].get()));
        p("HEJJ: ",static_cast<FunctionPointerType *>(program.typeHandler.ditypeCache[diDtype].get()));
        p("HEJ: ",program.typeHandler.sortedTypeDefs.back());
        return program.typeHandler.ditypeCache[diDtype].get();

        //[TODO] if metadata present replace sortedTypeDefs with 
}



Type *fixType(Program& program, const llvm::DIType *ditype) {
        

        if (!ditype)
            return program.typeHandler.voidType.get();
        // pointerTypes are cached:
        auto it = program.typeHandler.ditypeCache.find(ditype);
        if (it != program.typeHandler.ditypeCache.end()) {
            llvm::errs() << "DWELL DWELL DWELL DWELL\n";
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

        const llvm::DICompositeType* diCompType = llvm::dyn_cast<llvm::DICompositeType>(ditype);
        if ( diCompType && llvm::dwarf::DW_TAG_array_type == diCompType->getTag() ) {
            // WRONG NUMBER OF ELEMENTS
            uint32_t numberOfArrElem = 1;

            // https://llvm.org/doxygen/BPFAbstractMemberAccess_8cpp_source.html#l00334
            // why this heuristic?
            if (auto *Element = llvm::dyn_cast_or_null<llvm::DINode>(diCompType->getElements()[0])) {
                if (Element->getTag() == llvm::dwarf::DW_TAG_subrange_type) {
                    const llvm::DISubrange *SR = llvm::cast<llvm::DISubrange>(Element);
                    auto *CI = SR->getCount().dyn_cast<llvm::ConstantInt *>();
                    numberOfArrElem = CI->getSExtValue();
            return program.typeHandler.cachedDITypeInserter<ArrayType>(ditype, fixType(program, diCompType->getBaseType()),
                                                                 numberOfArrElem);
                }
                llvm::errs() << "WHOA?\n";
            }
        }
        // DIDerivedType is used to represent a type that is derived
        // from another type, such as a pointer, or typedef.
        const llvm::DIDerivedType* diDerivedType = llvm::dyn_cast<llvm::DIDerivedType>(ditype);
        if( diDerivedType && (diDerivedType->getTag() == llvm::dwarf::DW_TAG_pointer_type ||
                             diDerivedType->getTag() == llvm::dwarf::DW_TAG_typedef)) {
            // if (!diDerivedType->getBaseType()) /* <void *> " */{
            //     return program.typeHandler.cachedDITypeInserter<PointerType>(diDerivedType,
            //                                                  program.typeHandler.voidType.get());
            // }

            if (const llvm::DISubroutineType *diStype = llvm::dyn_cast<llvm::DISubroutineType>( diDerivedType->getBaseType() )) {
                return getFnctnPtrType(program, diDerivedType, diStype);
            }
            auto *innerType = fixType(program, diDerivedType->getBaseType());
            // const llvm::PointerType* PT;
            return program.typeHandler.cachedDITypeInserter<PointerType>(diDerivedType, innerType);
        }


        llvm::errs() << "Unknown type <"<< ditype->getTag() << ">! Terminating...\n";
        std::terminate();
        return nullptr;
}



        // what is llvm::DICompositeTypeArray ?
        // if ( (const llvm::DICompositeType* diCompType = llvm::dyn_cast<llvm::DICompositeType>(ditype)) &&
        //                                             llvm::dwarf::DW_TAG_array_type == diCompType->getTag() ) {
        //     auto y = std::make_unique<ArrayType>(fixType(program, diCompType->getBaseType()) ,
        //                                                     diCompType->getElements().size());
        //     //auto result = ty.get();
        //     //auto* inner = getType(PT->getPointerElementType());
        //     //makeCachedType<PointerType>(PT, inner)
        //     // program.typeHandler.makeCachedType<ArrayType>(type, fixType(program, diCompType->getBaseType()),
        //     //                                                         diCompType->getElements().size());
        //     // TODO Insert to typeCache.insert(it, std::make_pair(type, std::move(ty)));
        // }t

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

        if (auto t = fixType(program, localVar->getType())) {
        p("settingMetadataInfo for", "var",  variable->valueName, "\n");
            variable->setType(t);
        }
    }
}

void parseMetadataTypes(const llvm::Module* module, Program& program) {
    assert(program.isPassCompleted(PassType::CreateAllocas));
    // program.typeHandler.typeCache.clear();

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
            llvm::errs() << "    Global var name: " << Var->getName() << "\n";
            
            if ( program.getGlobalVar( &gvar ) ) {
                program.getGlobalVar( &gvar )->expr->setType(
                    fixType(program, Var->getType()));
                 llvm::errs() << "        found\n";
            } else
                llvm::errs() << " Global v missing, but debuginfo\n";
//   414       llvm::DIExpression *Expr = GVE->getExpression();
        }
    }
    program.addPass(PassType::ParseMetadataTypes);
}