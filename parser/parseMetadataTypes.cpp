#include "../core/Program.h"
#include "../core/Func.h"
#include "../core/Block.h"
#include "../type/Type.h"

#include <llvm/IR/Instruction.h>
#include <llvm/IR/IntrinsicInst.h>

#include <numeric>

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
        if (!ditype)
            return program.typeHandler.voidType.get();
        // pointerTypes are cached:
        auto it = program.typeHandler.ditypeCache.find(ditype);
        if (it != program.typeHandler.ditypeCache.end()) {
            return it->second.get();
        }
 
        // StructTypes are cached
        auto sit = program.typeHandler.StructTypeDiCache.find(ditype);
        if (sit != program.typeHandler.StructTypeDiCache.end()) {
            return sit->second;
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
        if ( diCompType && diCompType->getTag() == llvm::dwarf::DW_TAG_member) // type is a struct member
            return fixType(program, diCompType->getBaseType());
        if ( diCompType && llvm::dwarf::DW_TAG_array_type == diCompType->getTag() ) {

            std::vector<llvm::DINode *> elmnts(diCompType->getElements().begin(), diCompType->getElements().end());


            /* If there's an multidimensional array e.g. int arr_i[1][1][1];
             * llvm2c creates this type using recursion, so it's an array of
             * array of array of ints!
             * The code bellow simulate recursion as follows:
             * creates innermost_array
             * uses `accumulate` to create outter arrays with inner arrays
             * as childrens... we build MATRIOSHKA from innermost_array.
             */
            auto elmnt_rvrsd = llvm::reverse(elmnts);
            const llvm::DISubrange *SR = llvm::cast<llvm::DISubrange>(*(elmnt_rvrsd.begin())); // last element
            auto *CI = SR->getCount().dyn_cast<llvm::ConstantInt *>();
            auto innermost_array = program.typeHandler.cachedDITypeInserter<ArrayType>(
                                    ditype,
                                    fixType(program, diCompType->getBaseType()),
                                                                 CI->getSExtValue());

            return std::accumulate(++elmnt_rvrsd.begin(),
                            elmnt_rvrsd.end(),
                            innermost_array,
                            [&program](auto rght_arr, auto lft_arr){
                                if (const llvm::DISubrange *SR = llvm::cast<llvm::DISubrange>(lft_arr)) {
                                    auto *CI = SR->getCount().dyn_cast<llvm::ConstantInt *>();
                                    auto ptr = std::make_unique<ArrayType>(rght_arr, CI->getSExtValue());
                                    auto* result = ptr.get();
                                    program.typeHandler.diSubranges.push_back(std::move(ptr));
                                    return result;
                                }
                                p("Unexpected d(ebug)i(nfo)Type of array elements occured!\n");
                                std::terminate();
                            }
                );
        } else if (diCompType && llvm::dwarf::DW_TAG_structure_type == diCompType->getTag() ) {
            std::string strctName = diCompType->getName().data();
            auto strct = program.getStruct( "s_" + strctName );
            if( !strct )
                p("Unable to find struct with name: ", strctName, "\n"), std::terminate();

            if (strct->items.size() != diCompType->getElements().size()) {
                p("Struct and DIStruct has different number of attributes");
                std::terminate();
            }
            program.typeHandler.StructTypeDiCache[diCompType] = strct;
            size_t index = 0;
            while( index < strct->items.size()) {
                llvm::DIType *di_node = llvm::dyn_cast_or_null<llvm::DIType>(
                                                diCompType->getElements()[index]);
                strct->items[index].first = fixType(program, di_node);
                strct->items[index].second = di_node->getName().str();

                index++;
            }
            return strct;
        } else if (diCompType && llvm::dwarf::DW_TAG_union_type == diCompType->getTag()) {
            /* In LLVM there are no unions; there are only structs that can be cast into
            *  whichever type the front-end want to cast the struct into.
            *  from: https://mapping-high-level-constructs-to-llvm-ir.readthedocs.io/en/latest/basic-constructs/unions.html
            */
            std::string unionName = diCompType->getName().data();
            auto onion = program.getStruct( "u_" + unionName );
            if( !onion )
                p("Unable to find union with name: ", unionName, "\n"), std::terminate();

            // ASSUMPTION: union has always single type (in LLVMIR)
            size_t index = 0;
            while( index < onion->items.size()) {
                llvm::DIType *di_node = llvm::dyn_cast_or_null<llvm::DIType>(
                                                diCompType->getElements()[index]);
                if (onion->items.front().first->getKind() == fixType(program, di_node)->getKind()) {
                    onion->items.front().first = fixType(program, di_node);
                    onion->items.front().second = di_node->getName().str();
                    break;
                }
                index++;
            }
            return onion;
        }
        // DIDerivedType is used to represent a type that is derived
        // from another type, such as a pointer, or typedef.
        const llvm::DIDerivedType* diDerivedType = llvm::dyn_cast<llvm::DIDerivedType>(ditype);
        if (diDerivedType && diDerivedType->getTag() == llvm::dwarf::DW_TAG_member) return fixType(program, diDerivedType->getBaseType());
        if (diDerivedType && diDerivedType->getTag() == llvm::dwarf::DW_TAG_const_type) return fixType(program, diDerivedType->getBaseType());
            // Commented because:
            // 1. we cannot distinguish between "* const *" and "const **"
            // 2. has no effect on semantics
            // auto ft = fixType(program, diDerivedType->getBaseType());
            // ft->isConst = true;
            // return ft;
        if( diDerivedType && (diDerivedType->getTag() == llvm::dwarf::DW_TAG_pointer_type)) {

            if (const llvm::DISubroutineType *diStype = llvm::dyn_cast<llvm::DISubroutineType>( diDerivedType->getBaseType() )) {
                return getFnctnPtrType(program, diDerivedType, diStype);
            }
            auto *innerType = fixType(program, diDerivedType->getBaseType());
            return program.typeHandler.cachedDITypeInserter<PointerType>(diDerivedType, innerType);
        } else if (diDerivedType && (diDerivedType->getTag() == llvm::dwarf::DW_TAG_typedef)) {
            return fixType(program, diDerivedType->getBaseType());
        }


        p("Unknown type tag:<", ditype->getTag(), "> name<", ditype->getName(), ">\n");
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

        if (auto t = fixType(program, localVar->getType())) {
            if (t->getKind() != variable->getType()->getKind()) {
                llvm::errs() << "The type of this variable:" << localVar->getName()
                             << " specified by the user differs from the type in llvmir.\n";
                return;
            }
            variable->setType(t);
        }
    }
}

void parseMetadataTypes(const llvm::Module* module, Program& program) {
    assert(program.isPassCompleted(PassType::CreateAllocas));

    for (const llvm::GlobalVariable& gvar : module->globals()) {
        // Vector bellow is always holding only current gvar,
        // thus for-cycle bellow always has only one iteration.
        llvm::SmallVector<llvm::DIGlobalVariableExpression *, 1> GVs;
        gvar.getDebugInfo(GVs);
        for (auto *GVE : GVs) {
            llvm::DIVariable *Var = GVE->getVariable();
            if ( program.getGlobalVar( &gvar ) ) {
            p("Setting type for Gvar: ",  gvar.getName(), "\n");
                program.getGlobalVar( &gvar )->expr->setType(
                    fixType(program, Var->getType()));
            p("type set for Gvar: ", gvar.getName(), "\n");
            } else
                llvm::errs() << " Global v missing, but debuginfo\n";
        }
    }

    // We need to set the right type also for function arguments!
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

       if (!func || (func->isDeclaration) || /* noMetadata */ !function.getSubprogram()) continue;

       bool isStructInArgs = std::any_of(function.getSubprogram()->getType()->getTypeArray().begin(),
                   function.getSubprogram()->getType()->getTypeArray().end(),
                   [](auto argType){return argType && llvm::dyn_cast<llvm::DICompositeType>(argType) &&
                                            ((llvm::dyn_cast<llvm::DICompositeType>(argType)->getTag() ==
                                            llvm::dwarf::DW_TAG_structure_type) ||
                                            (llvm::dyn_cast<llvm::DICompositeType>(argType)->getTag() ==
                                            llvm::dwarf::DW_TAG_union_type));});
        if (isStructInArgs) {
            p("parseMetadataTypes: looks like struct is passed by value to this function: ", func->name, "\n");
            continue;
        }
        func->returnType = fixType(program,
                        *function.getSubprogram()->getType()->getTypeArray().begin()
                                  );
        size_t indx = 0;
        while( indx < func->parameters.size() ){
            func->parameters[indx]->setType(
                fixType(program,
                        function.getSubprogram()->getType()->getTypeArray()[++indx]
                       )
                // ++indx is here because the first element from a TypeArray
                // is the function return type.
            );
        }
    }
    program.addPass(PassType::ParseMetadataTypes);
}