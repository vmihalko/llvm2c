#include "TypeHandler.h"

#include "llvm/IR/DerivedTypes.h"

#include "../core/Program.h"

#include <boost/lambda/lambda.hpp>

Type* TypeHandler::getType(const llvm::Type* type) {
    if (typeDefs.find(type) != typeDefs.end()) {
        return typeDefs[type].get();
    }
    
    auto it = typeCache.find(type);
    if (it != typeCache.end()) {
        return it->second.get();
    }

    if (type->isArrayTy()) {
        auto ty = std::make_unique<ArrayType>(getType(type->getArrayElementType()), type->getArrayNumElements());
        typeCache.insert(it, std::make_pair(type, std::move(ty)));
    }

    if (type->isVoidTy()) {
        auto ty = std::make_unique<VoidType>();
        typeCache.insert(it, std::make_pair(type, std::move(ty)));
    }

    if (type->isIntegerTy()) {
        std::unique_ptr<Type> ty;
        const auto intType = static_cast<const llvm::IntegerType*>(type);
        if (intType->getBitWidth() == 1) {
            return std::make_unique<IntType>(true);
        }

        if (intType->getBitWidth() <= 8) {
            return std::make_unique<CharType>(true);
        }

        if (intType->getBitWidth() <= 16) {
            return std::make_unique<ShortType>(true);
        }

        if (intType->getBitWidth() <= 32) {
            return std::make_unique<IntType>(true);
        }

        if (intType->getBitWidth() <= 64) {
            return std::make_unique<LongType>(true);
        }

        return std::make_unique<Int128>();
    }

    if (type->isFloatTy()) {
        return std::make_unique<FloatType>();
    }

    if (type->isDoubleTy()) {
        return std::make_unique<DoubleType>();
    }

    if (type->isX86_FP80Ty()) {
        return std::make_unique<LongDoubleType>();
    }

    if (type->isPointerTy()) {
        const llvm::PointerType* PT = llvm::cast<const llvm::PointerType>(type);

        if (const llvm::FunctionType* FT = llvm::dyn_cast_or_null<llvm::FunctionType>(PT->getPointerElementType())) {
            std::vector<std::string> params;
            if (FT->getNumParams() == 0) {
                params.push_back("void");
            } else {
                std::string param;
                //parsing params to string
                for (unsigned i = 0; i < FT->getNumParams(); i++) {
                    auto paramType = getType(FT->getParamType(i));
                    param = paramType->toString();

                    if (auto PT = llvm::dyn_cast_or_null<PointerType>(paramType)) {
                        if (PT->isArrayPointer) {
                            param += " (";
                            for (unsigned i = 0; i < PT->levels; i++) {
                                param += "*";
                            }
                            param += ")" + PT->sizes;
                        }
                    }

                    if (auto AT = llvm::dyn_cast_or_null<ArrayType>(paramType)) {
                        param += AT->sizeToString();
                    }

                    params.push_back(param);
                }

                if (FT->isVarArg()) {
                    params.push_back("...");
                }
            }

            std::string paramsToString = "(";
            bool first = true;
            for (const auto& param : params) {
                if (!first) {
                    paramsToString += ", ";
                }
                first = false;

                paramsToString += param;
            }
            paramsToString += ")";

            typeDefs[type] = std::make_unique<FunctionPointerType>(getType(FT->getReturnType())->toString() + "(*", getTypeDefName(), ")" + paramsToString);
            sortedTypeDefs.push_back(static_cast<FunctionPointerType*>(typeDefs[type].get()));
            return typeDefs[type].get();
        }

        return std::make_unique<PointerType>(getType(PT->getPointerElementType()));
    }

    if (type->isStructTy()) {
        const llvm::StructType* structType = llvm::cast<const llvm::StructType>(type);

        if (!structType->hasName()) {
            program->createNewUnnamedStruct(structType);
            return std::make_unique<StructType>(program->getStruct(structType)->name);
        }

        if (structType->getName().str().compare("struct.__va_list_tag") == 0) {
            return std::make_unique<StructType>("__va_list_tag");
        }

        return std::make_unique<StructType>(getStructName(structType->getName().str()));
    }

    return nullptr;
}

Type* TypeHandler::getBinaryType(Type* left, Type* right) {
    if (left != nullptr) 
        return left;

    return right;
}

std::string TypeHandler::getStructName(const std::string& structName) {
    std::string name = structName;
    std::replace(name.begin(), name.end(), '.', '_');

    if (name.substr(0, 6).compare("struct") == 0) {
        name.erase(0, 7);
        name = "s_" + name;
    } else {
        //union
        name.erase(0, 6);
        name = "u_" + name;
    }

    if (name.compare("s___va_list_tag") == 0) {
        name = "__va_list_tag";
    }

    return name;
}
