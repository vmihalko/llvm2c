#include "TypeHandler.h"

#include "llvm/IR/DerivedTypes.h"

#include "../core/Program.h"

 unsigned TypeHandler::getTypeDefNumber() {
        llvm::errs() << "call #" << typeDefCountForMetadata << "\n";
        return typeDefCountForMetadata++;
        // std::string ret = "typeDef_" + std::to_string(typeDefCount);
        // typeDefCount++;
        // if (metadata)
        //     return "typeDef_" + std::to_string(++typeDefCountForMetadata);
        // return ret;
    }

void printMapKeys(const std::unordered_map<const llvm::Type *, std::unique_ptr<Type>> &map)
{
    // Iterate over the map using a const_iterator
    for (auto it = map.begin(); it != map.end(); ++it)
    {
        // Print the key using the LLVM Type's print method
        it->first->print(llvm::outs());
        llvm::outs() << "\n";
    }
}

void printTypeAddress(const llvm::Type *type)
{
    // Use the & operator to get the address of the pointer
    void *address = static_cast<void *>(const_cast<llvm::Type *>(type));

    // Print the address using std::cout
    llvm::errs() << "Address of type " << type << ": " << address << "\n";
}

Type *TypeHandler::getType(const llvm::Type *type)
{
    if (typeDefs.find(type) != typeDefs.end())
    {
        return typeDefs[type].get();
    }

    auto it = typeCache.find(type); // DONE
    if (it != typeCache.end())
    {
        llvm::errs() << "WELL WELL WELL WELL\n";
        return it->second.get();
    }

    if (type->isArrayTy())
    {
        auto ty = std::make_unique<ArrayType>(getType(type->getArrayElementType()), type->getArrayNumElements());
        auto result = ty.get();
        typeCache.insert(it, std::make_pair(type, std::move(ty)));
        return result;
    }

    if (type->isVoidTy()) // DONE
    {
        return voidType.get();
    }

    if (type->isIntegerTy())
    {
        std::unique_ptr<Type> ty;
        const auto intType = static_cast<const llvm::IntegerType *>(type);
        if (intType->getBitWidth() == 1)
        {
            return uint.get();
        }

        if (intType->getBitWidth() <= 8)
        {
            return uchar.get();
        }

        if (intType->getBitWidth() <= 16)
        {
            return ushort.get();
        }

        if (intType->getBitWidth() <= 32)
        {
            return uint.get();
        }

        if (intType->getBitWidth() <= 64)
        {
            return ulong.get();
        }

        return int128.get();
    }

    if (type->isFloatTy())
    {
        return floatType.get();
    }

    if (type->isDoubleTy())
    {
        return doubleType.get();
    }

    if (type->isX86_FP80Ty())
    {
        return longDoubleType.get();
    }

    if (type->isPointerTy())
    {
        printTypeAddress(type);
        const llvm::PointerType *PT = llvm::cast<const llvm::PointerType>(type);

        if (const llvm::FunctionType *FT = llvm::dyn_cast_or_null<llvm::FunctionType>(PT->getPointerElementType()))
        {
            std::vector<std::string> params;
            if (FT->getNumParams() == 0)
            {
                params.push_back("void");
            }
            else
            {
                std::string param;
                // parsing params to string
                for (unsigned i = 0; i < FT->getNumParams(); i++)
                {
                    auto paramType = getType(FT->getParamType(i));
                    param = paramType->toString();

                    if (auto PT = llvm::dyn_cast_or_null<PointerType>(paramType))
                    {
                        if (PT->isArrayPointer)
                        {
                            param += " (";
                            for (unsigned i = 0; i < PT->levels; i++)
                            {
                                param += "*";
                            }
                            param += ")" + PT->sizes;
                        }
                    }

                    if (auto AT = llvm::dyn_cast_or_null<ArrayType>(paramType))
                    {
                        param += AT->sizeToString();
                    }

                    params.push_back(param);
                }

                if (FT->isVarArg())
                {
                    params.push_back("...");
                }
            }

            std::string paramsToString = "(";
            bool first = true;
            for (const auto &param : params)
            {
                if (!first)
                {
                    paramsToString += ", ";
                }
                first = false;

                paramsToString += param;
            }
            paramsToString += ")";

            typeDefs[type] = std::make_unique<FunctionPointerType>(getType(FT->getReturnType())->toString() + "(*", getTypeDefName(), ")" + paramsToString);
            sortedTypeDefs.push_back(static_cast<FunctionPointerType *>(typeDefs[type].get()));
            return typeDefs[type].get();
        }

        auto *inner = getType(PT->getPointerElementType());
        auto *t = makeCachedType<PointerType>(PT, inner);
        llvm::errs() << "NOW\n";
        printMapKeys(typeCache);
        llvm::errs() << "THEN\n";
        return t;
    }

    if (type->isStructTy())
    {
        const llvm::StructType *structType = llvm::cast<const llvm::StructType>(type);

        auto *strct = program->getStruct(structType);

        if (!strct)
        {
            if (structType->getStructName() == "")
            {
                program->createNewUnnamedStruct(structType);
                strct = program->getStruct(structType);
            }
        }

        return strct;
    }

    return nullptr;
}

Type *TypeHandler::getBinaryType(Type *left, Type *right)
{
    if (left != nullptr)
        return left;

    return right;
}

std::string TypeHandler::getStructName(const std::string &structName)
{
    std::string name = structName;
    std::replace(name.begin(), name.end(), '.', '_');

    if (name.substr(0, 6).compare("struct") == 0)
    {
        name.erase(0, 7);
        name = "s_" + name;
    }
    else
    {
        // union
        name.erase(0, 6);
        name = "u_" + name;
    }

    if (name.compare("s___va_list_tag") == 0)
    {
        name = "__va_list_tag";
    }

    return name;
}

Type *TypeHandler::pointerTo(Type *type)
{
    auto it = pointerTypes.find(type);
    if (it != pointerTypes.end())
    {
        return it->second.get();
    }

    auto up = std::make_unique<PointerType>(type);
    Type *result = up.get();
    pointerTypes.insert(it, std::make_pair(type, std::move(up)));
    return result;
}

IntegerType *TypeHandler::toggleSignedness(IntegerType *ty)
{

#define TYPES(unsignedType, signedType) \
    do                                  \
    {                                   \
        if (ty == signedType.get())     \
            return unsignedType.get();  \
        if (ty == unsignedType.get())   \
            return signedType.get();    \
    } while (0);

    TYPES(uint, sint);
    TYPES(uchar, schar);
    TYPES(ushort, sshort);
    TYPES(ulong, slong);

#undef TYPES
    return ty;
}

IntegerType *TypeHandler::setSigned(IntegerType *ty)
{
#define TYPES(unsignedType, signedType) \
    do                                  \
    {                                   \
        if (ty == unsignedType.get())   \
            return signedType.get();    \
    } while (0);

    TYPES(uint, sint);
    TYPES(uchar, schar);
    TYPES(ushort, sshort);
    TYPES(ulong, slong);

#undef TYPES
    return ty;
}

IntegerType *TypeHandler::setUnsigned(IntegerType *ty)
{
#define TYPES(unsignedType, signedType) \
    do                                  \
    {                                   \
        if (ty == signedType.get())     \
            return unsignedType.get();  \
    } while (0);

    TYPES(uint, sint);
    TYPES(uchar, schar);
    TYPES(ushort, sshort);
    TYPES(ulong, slong);

#undef TYPES
    return ty;
}
