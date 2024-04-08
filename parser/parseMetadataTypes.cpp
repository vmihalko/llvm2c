#include "../core/Program.h"
#include "../core/Func.h"
#include "../core/Block.h"
#include "../type/Type.h"

#include <optional>
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

#define CHAIN(TYPE, MEMBER) ((TYPE) ? (TYPE)->MEMBER : nullptr)

std::optional<Type *> fixType(Program& program, const llvm::DIType *ditype, const llvm::Type *anonGVName);

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

std::optional<Type *> getFnctnPtrType(Program& program, const llvm::DIDerivedType *diDtype,
                                         const llvm::DISubroutineType *diStype,
                                         const llvm::FunctionType *anonGVName) {

        auto t = fixType(program, (*diStype->getTypeArray().begin()), CHAIN(anonGVName, getReturnType()));
        if (!t.has_value())
            return {};
        std::string rtrnType = t.value()->toString();

        std::vector<std::string> fnctnTypesOfArgs;
        for (size_t index = 0; index < diStype->getTypeArray().size() - 1; index++) {
            auto t = fixType(program, diStype->getTypeArray()[index + 1], CHAIN(anonGVName, getFunctionParamType(index)));
            if (!t.has_value())
                return {};
            fnctnTypesOfArgs.push_back(t.value()->toString());
        }

        std::string fnctnTypesOfArgsString;
        vectorToString( fnctnTypesOfArgs, fnctnTypesOfArgsString); // e.g. "(int, doubl, char *)"

        auto it = program.typeHandler.ditypeCache.find(diDtype);
        if (it != program.typeHandler.ditypeCache.end()) {
            return it->second.get();
        }

        auto nthTypeDef = program.typeHandler.getTypeDefNumber();
        program.typeHandler.ditypeCache[diDtype] = std::make_unique<FunctionPointerType>(
                    rtrnType + "(*",  "typeDef_" + std::to_string(nthTypeDef), ")" + fnctnTypesOfArgsString);
        program.typeHandler.sortedTypeDefs[nthTypeDef] = static_cast<FunctionPointerType *>(program.typeHandler.ditypeCache[diDtype].get());
        return program.typeHandler.ditypeCache[diDtype].get();
}

std::optional<Type *> fixType(Program& program, const llvm::DIType *ditype, const llvm::Type * anonGVName)  {
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
                if(tbasic->getEncoding() == llvm::dwarf::DW_ATE_boolean)
                    return program.typeHandler.uint.get();
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
                if (signedness)
		       return program.typeHandler.slonglong.get();
	        return program.typeHandler.ulonglong.get();
            }
        }
        const llvm::DICompositeType* diCompType = llvm::dyn_cast<llvm::DICompositeType>(ditype);
        if ( diCompType && diCompType->getTag() == llvm::dwarf::DW_TAG_member) // type is a struct member
            return fixType(program, diCompType->getBaseType(), anonGVName);
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
            int64_t array_size = 0;
            if (CI) array_size = CI->getSExtValue();

            auto arrayBaseType = anonGVName && anonGVName->isArrayTy() ? CHAIN(anonGVName, getArrayElementType()) : nullptr;
            while( arrayBaseType && arrayBaseType->isArrayTy() )
                arrayBaseType = arrayBaseType->getArrayElementType();
            auto t = fixType(program, diCompType->getBaseType(), arrayBaseType);
            if ( !t.has_value() )
                return {};
            auto ptr = std::make_unique<ArrayType>(t.value(), array_size);
            auto* innermost_array = ptr.get();
            program.typeHandler.diSubranges.push_back(std::move(ptr));

            auto *outermost_array = std::accumulate(++elmnt_rvrsd.begin(),
                            elmnt_rvrsd.end(),
                            innermost_array,
                            [&program](auto rght_arr, auto lft_arr){
                                if (const llvm::DISubrange *SR = llvm::cast<llvm::DISubrange>(lft_arr)) {
                                    auto *CI = SR->getCount().dyn_cast<llvm::ConstantInt *>();
                                    int64_t array_size = 0;
                                    if (CI) array_size = CI->getSExtValue();
                                    auto ptr = std::make_unique<ArrayType>(rght_arr, array_size);
                                    auto* result = ptr.get();
                                    program.typeHandler.diSubranges.push_back(std::move(ptr));
                                    return result;
                                }
                                p("Unexpected d(ebug)i(nfo)Type of array elements occured!\n");
                                std::terminate();
                            }
                );
            // We want to associate the complete array type with ditype
            // to do so, we move it from diSubranges.back() to diTypeCache
            program.typeHandler.ditypeCache[ditype] = std::move(program.typeHandler.diSubranges.back());
            program.typeHandler.diSubranges.pop_back();

            return outermost_array;

        } else if (diCompType && llvm::dwarf::DW_TAG_structure_type == diCompType->getTag() ) {
            if (anonGVName && !anonGVName->isStructTy())
                anonGVName = nullptr;
            // if (diCompType->getName().empty() && (!anonGVName || !anonGVName->isStructTy()))
            //     return {};

            if (anonGVName && anonGVName->getStructName().empty()) // anonymous struct
                return program.unnamedStructs[llvm::cast<llvm::StructType>(anonGVName)].get();

            std::string strctName = diCompType->getName().empty() && anonGVName
                     ? TypeHandler::getStructName(anonGVName->getStructName().str())
                     : "s_" + diCompType->getName().str();

            auto strct = program.getStruct( strctName );
            if ( !strct && anonGVName) // struct with name: diCompType->getName().str() wasn't found
                strct = program.getStruct( TypeHandler::getStructName(anonGVName->getStructName().str()) );
            if( !strct ) {
                p("Unable to find struct with name: ", strctName, "\n");
                return {};
            }
                //, std::terminate();

            if (strct->items.size() != diCompType->getElements().size()) {
                p("Struct and DIStruct has different number of attributes.");
                p("Error or struct with bit-fields");
                return {};
            }
            program.typeHandler.StructTypeDiCache[diCompType] = strct;
            size_t index = 0;
            while( index < strct->items.size()) {
                llvm::DIType *di_node = llvm::dyn_cast_or_null<llvm::DIType>(
                                                diCompType->getElements()[index]);
                strct->items[index].first = fixType(program,
                                                        di_node,
                                                        CHAIN(anonGVName,
                                                        getStructElementType(index))).value_or(
                                                                                strct->items[index].first);

                if (!di_node->getName().empty())
                    strct->items[index].second = di_node->getName().str();

                index++;
            }
            return strct;
        } else if (diCompType && llvm::dwarf::DW_TAG_union_type == diCompType->getTag()) {
            if (anonGVName && !anonGVName->isStructTy())
                anonGVName = nullptr;
            // if (diCompType->getName().empty() && (!anonGVName || !anonGVName->isStructTy()))
            //     return {};

            if (anonGVName && anonGVName->getStructName().empty()) // anonymous union
                return program.unnamedStructs[llvm::cast<llvm::StructType>(anonGVName)].get();
            /* In LLVM there are no unions; there are only structs that can be cast into
            *  whichever type the front-end want to cast the struct into.
            *  from: https://mapping-high-level-constructs-to-llvm-ir.readthedocs.io/en/latest/basic-constructs/unions.html
            */
            std::string unionName = diCompType->getName().empty() && anonGVName
                     ? TypeHandler::getStructName(anonGVName->getStructName().str())
                     : "u_" + diCompType->getName().str();

            auto onion = program.getStruct( unionName );
            if ( !onion && anonGVName) // union with name: diCompType->getName().str() wasn't found
                onion = program.getStruct( TypeHandler::getStructName(anonGVName->getStructName().str()) );
            if( !onion ) {
                p("Unable to find union with name: ", unionName, "\n");
                return {};
            }

            // ASSUMPTION: union has always single type (in LLVMIR)
            auto r = std::max_element(diCompType->getElements().begin(), diCompType->getElements().end(), [](llvm::DINode *a, llvm::DINode *b){
                return llvm::dyn_cast<llvm::DIType>( a )->getSizeInBits() <
                       llvm::dyn_cast<llvm::DIType>( b )->getSizeInBits();});
            if (auto t = fixType(program, llvm::dyn_cast<llvm::DIType>(*r), nullptr)) {
                if (t.has_value() && onion->items.front().first->getKind() == t.value()->getKind()) {
                    onion->items.front().first = t.value();
                    return onion;
                }
            }
            return {};
        } else if (diCompType && llvm::dwarf::DW_TAG_enumeration_type == diCompType->getTag()) {
            //TODO: llvm2c decompiles enum as a global variable
            // it should be easy to add a support for enum...
            return program.typeHandler.uint.get();
        }
        // DIDerivedType is used to represent a type that is derived
        // from another type, such as a pointer, or typedef.
        // if (diDerivedType && diDerivedType->getTag() == llvm::dwarf::DW_TAG_member) return fixType(program, diDerivedType->getBaseType(), anonGVName);
        const llvm::DIDerivedType* diDerivedType = llvm::dyn_cast<llvm::DIDerivedType>(ditype);
        if (diDerivedType && (   diDerivedType->getTag() == llvm::dwarf::DW_TAG_const_type
                              || diDerivedType->getTag() == llvm::dwarf::DW_TAG_restrict_type
                              || diDerivedType->getTag() == llvm::dwarf::DW_TAG_member
                              || diDerivedType->getTag() == llvm::dwarf::DW_TAG_volatile_type )) {

            return fixType(program, diDerivedType->getBaseType(), anonGVName);
        }
            // Commented because:
            // 1. we cannot distinguish between "* const *" and "const **"
            // 2. has no effect on semantics
            // auto ft = fixType(program, diDerivedType->getBaseType());
            // ft->isConst = true;
            // return ft;
        if( diDerivedType && (diDerivedType->getTag() == llvm::dwarf::DW_TAG_pointer_type)) {
            if (anonGVName && !anonGVName->isPointerTy())
                anonGVName = nullptr;

            if (!diDerivedType->getBaseType())
                return program.typeHandler.cachedDITypeInserter<PointerType>(diDerivedType,
                            program.typeHandler.voidType.get());

            if (const llvm::DISubroutineType *diStype = llvm::dyn_cast<llvm::DISubroutineType>( diDerivedType->getBaseType() )) {
                if (anonGVName && anonGVName->getPointerElementType()->isFunctionTy())
                    return getFnctnPtrType(program, diDerivedType, diStype, llvm::cast_or_null<llvm::FunctionType>(CHAIN(anonGVName, getPointerElementType())));
                if (!anonGVName)
                    return getFnctnPtrType(program, diDerivedType, diStype, llvm::cast_or_null<llvm::FunctionType>(CHAIN(anonGVName, getPointerElementType())));
                return {};
            }
            auto t = fixType(program, diDerivedType->getBaseType(), CHAIN(anonGVName, getPointerElementType()));
            if ( !t.has_value() )
                return {};
            auto *innerType = t.value();

            auto it = program.typeHandler.ditypeCache.find(ditype);
            if (it != program.typeHandler.ditypeCache.end()) {
                return it->second.get();
            }
            return program.typeHandler.cachedDITypeInserter<PointerType>(diDerivedType, innerType);
        } else if (diDerivedType && (diDerivedType->getTag() == llvm::dwarf::DW_TAG_typedef)) {
            if (!diDerivedType->getBaseType())
                return program.typeHandler.cachedDITypeInserter<PointerType>(diDerivedType,
                            program.typeHandler.voidType.get());
            if (const llvm::DISubroutineType *diStype = llvm::dyn_cast<llvm::DISubroutineType>( diDerivedType->getBaseType() )) {
            if (anonGVName && anonGVName->isFunctionTy())
                return getFnctnPtrType(program, diDerivedType, diStype, llvm::cast_or_null<llvm::FunctionType>(anonGVName));
            if (!anonGVName)
                return getFnctnPtrType(program, diDerivedType, diStype, llvm::cast_or_null<llvm::FunctionType>(anonGVName));
            return {};
            }
            return fixType(program, diDerivedType->getBaseType(), anonGVName);
        }

        // if (const llvm::DISubroutineType *diStype = llvm::dyn_cast<llvm::DISubroutineType>( ditype )) {
        //     return getFnctnPtrType(program, diDerivedType, diStype);
        // }

        p("Unknown type tag:<", ditype->getTag(), "> name<", ditype->getName(), ">\n");
        std::terminate();
        return {};
}

static void setMetadataInfo(Program& program, const llvm::CallInst* ins, Block* block) {
    llvm::Metadata* md = llvm::dyn_cast_or_null<llvm::MetadataAsValue>(ins->getOperand(0))->getMetadata();
    llvm::Value* referredVal = llvm::cast<llvm::ValueAsMetadata>(md)->getValue();
    if (auto a = llvm::dyn_cast<llvm::DIArgList>(md)){
	    return;
    llvm::errs() << "This is myValue: ";
    referredVal->print(llvm::errs());
    llvm::errs() << "\n";
    }
    //referredVal->print(llvm::errs());
    Expr* referred = block->func->getExpr(referredVal);
    if (false && !referred) {
	    llvm::errs() << " NULLhere1\n";
        if (const auto* ins = llvm::dyn_cast_or_null<llvm::Instruction>(referredVal)) {
	    llvm::errs() << "2\n";
            if (ins->hasNUses(1)) {
	    llvm::errs() << "3\n";
                if (const llvm::Value* inlinedIntoValue = llvm::dyn_cast_or_null<llvm::Value>(*ins->user_begin())) {
	    llvm::errs() << "4\n";
    inlinedIntoValue->print(llvm::errs());
	    llvm::errs() << " before 4\n";
                    Expr* referred = block->func->getExpr(inlinedIntoValue);
                    if (referred)
                        llvm::errs() << "Hurray " << inlinedIntoValue << "\n";
            }
        }
    }
}
/*
    if(!referred) {
	    llvm::errs()<< "NOT IN EXPRS?!";
    referredVal->print(llvm::errs());
	    llvm::errs()<< "NULL\n"; return;
    }
*/
    if (auto* re = llvm::dyn_cast_or_null<RefExpr>(referred)) {
        referred = re->expr;
    }

    if (Value* variable = llvm::dyn_cast_or_null<Value>(referred)) {
	referredVal->print(llvm::errs());
        if (llvm::isa<llvm::Argument>(referredVal))
            // do later
            return;
        if (llvm::isa<llvm::GlobalVariable>(referredVal))
	    return;
	llvm::Type* AT = referredVal->getType(); //llvm::dyn_cast<llvm::AllocaInst>(referredVal)->getAllocatedType();
        llvm::Metadata* varMD = llvm::dyn_cast_or_null<llvm::MetadataAsValue>(ins->getOperand(1))->getMetadata();
        llvm::DILocalVariable* localVar = llvm::dyn_cast_or_null<llvm::DILocalVariable>(varMD);

        if (auto t = fixType(program, localVar->getType(), AT)) {
            if (!t.has_value() || t.value()->getKind() != variable->getType()->getKind()) { // TODO UNION != STRUCT
                llvm::errs() << "The type of this variable:" << localVar->getName()
                             << " specified by the user differs from the type in DIinfo.\n";
                return;
            }
            variable->setType(t.value());
            program.metadatedVars.insert(variable->valueName);
        }
	return;
    } else if (CallExpr* callVar = llvm::dyn_cast_or_null<CallExpr>(referred)) {
		llvm::Type* AT = referredVal->getType(); //llvm::dyn_cast<llvm::AllocaInst>(referredVal)->getAllocatedType();
		llvm::Metadata* varMD = llvm::dyn_cast_or_null<llvm::MetadataAsValue>(ins->getOperand(1))->getMetadata();
		llvm::DILocalVariable* localVar = llvm::dyn_cast_or_null<llvm::DILocalVariable>(varMD);

		auto t = fixType(program, localVar->getType(), AT);
		    if (!t.has_value() || t.value()->getKind() != callVar->getType()->getKind()) { // TODO UNION != STRUCT
			llvm::errs() << "The type of this variable:" << localVar->getName()
				     << " specified by the user differs from the type in DIinfo.\n";
			return;
		    } else {
		        callVar->setType(t.value());
		    }
	return;
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
            if ( auto gv = program.getGlobalVar( &gvar ) ) {
                if (auto t = fixType(program, Var->getType(), gvar.getValueType())) {
                    if (t.has_value() && t.value()->getKind() == gv->expr->getType()->getKind()) {
                        program.getGlobalVar( &gvar )->expr->setType(t.value());
                    }
                } else
                    llvm::errs() << " Global Var missing, but debuginfo occured\n";
            }
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
                    if (CI->getCalledFunction() && (CI->getIntrinsicID() == llvm::Intrinsic::dbg_declare ||
					    CI->getIntrinsicID() == llvm::Intrinsic::dbg_value) ) {
                        setMetadataInfo(program, CI, myBlock);
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
            p("parseMetadataTypes: looks like struct is passed by value to this function. Nothing to do in: ", func->name, "\n");
            continue;
        }
        func->returnType = fixType(program,
                        *function.getSubprogram()->getType()->getTypeArray().begin(),
                        function.getReturnType()
                                  ).value_or( func->returnType );
        size_t indx = 0;
        while( indx < func->parameters.size() ){
            func->parameters[indx]->setType(
                fixType(program,
                        function.getSubprogram()->getType()->getTypeArray()[indx + 1],
                        function.getFunctionType()->getParamType(indx)).value_or( func->parameters[indx]->getType() ));
                // ++indx is here because the first element from a TypeArray
                // is the function return type.
            ++indx;
        }
    }
    program.addPass(PassType::ParseMetadataTypes);
}
