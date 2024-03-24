#pragma once

#include "Type.h"
#include "../expr/Expr.h"

#include "llvm/IR/Type.h"
#include "llvm/ADT/DenseMap.h"
#include <llvm/IR/Module.h>

#include <memory>
#include <vector>
#include <unordered_map>

class Program;

class TypeHandler {
private:
    template<typename T>
    using uptr = std::unique_ptr<T>;

public:
    Program* program;
    llvm::DenseMap<const llvm::Type*, std::unique_ptr<Type>> typeDefs; //map containing typedefs
    llvm::DenseMap<const llvm::DIType*, std::unique_ptr<Type>> ditypeDefs; //map containing ditypedefs
    std::unordered_map<const llvm::Type*, std::unique_ptr<Type>> typeCache;
    std::unordered_map<const llvm::DIType*, std::unique_ptr<Type>> ditypeCache;
    std::unordered_map<const llvm::DISubrange*, std::unique_ptr<Type>> diSubrangeCache;
    std::unordered_map<const llvm::DIType*, StructType*> StructTypeDiCache;
    std::vector<std::unique_ptr<Type>> diSubranges;

    // key = T, value = Type representing pointer to T
    std::unordered_map<Type*, uptr<Type>> pointerTypes;

    unsigned typeDefCount = 0; //variable used for creating new name for typedef
    unsigned typeDefCountForMetadata = 0;

    /**
     * @brief getTypeDefName Creates new name for a typedef.
     * @return String containing new name for a typedef
     */
    std::string getTypeDefName() {
        std::string ret = "typeDef_" + std::to_string(typeDefCount);
        typeDefCount++;
        return ret;
    }
    /**
     * @brief getTypeDefNumber returns a unique typedef counter
     * @return unique typedef counter
     */
     unsigned getTypeDefNumber() {
        return typeDefCountForMetadata++;
    }

    /**
     * @brief makeCachedType Is putting together, during recursion,
     * saved types.
     * @return Type to which we are pointing
     */
    template<typename T, typename ...Args>
    Type* makeCachedType(const llvm::Type* ty, Args&&... args) {
        auto ptr = std::make_unique<T>(std::forward<Args>(args)...);
        auto* result = ptr.get();
        typeCache[ty] = std::move(ptr);
        return result;
    }

    template<typename T, typename ...Args>
    Type* cachedDITypeInserter(const llvm::DIType *ditype, Args&&... args) {
        auto ptr  = std::make_unique<T>(std::forward<Args>(args)...);
        auto *result = ptr.get();
        if (ditypeCache[ditype]) {
            llvm::errs() << "diType already cached! Terminating...\n";
            std::terminate();
        }
        ditypeCache[ditype] = std::move(ptr);
        return result;
    }

    std::vector<const FunctionPointerType*> sortedTypeDefs; //vector of sorted typedefs, used in output

    // basic C types
    uptr<IntType> uint = std::make_unique<IntType>(true);
    uptr<CharType> uchar = std::make_unique<CharType>(true);
    uptr<ShortType> ushort = std::make_unique<ShortType>(true);
    uptr<LongType> ulong = std::make_unique<LongType>(true);

    uptr<IntType> sint = std::make_unique<IntType>(false);
    uptr<CharType> schar = std::make_unique<CharType>(false);
    uptr<ShortType> sshort = std::make_unique<ShortType>(false);
    uptr<LongType> slong = std::make_unique<LongType>(false);
    uptr<LongLongType> slonglong = std::make_unique<LongLongType>(false);

    uptr<Int128> int128 = std::make_unique<Int128>();
    uptr<VoidType> voidType = std::make_unique<VoidType>();

    uptr<FloatType> floatType = std::make_unique<FloatType>();
    uptr<DoubleType> doubleType = std::make_unique<DoubleType>();
    uptr<LongDoubleType> longDoubleType = std::make_unique<LongDoubleType>();


    TypeHandler(Program* program)
        : program(program) { 
    }
    /**
     * @brief getDIType Transforms llvm::DIType into corresponding Type object
     * @param type llvm::DIType for transformation
     * @return unique_ptr to corresponding Type object
     */
    Type* getTypeFromDI(const llvm::DIType* type);

    /**
     * @brief getType Transforms llvm::Type into corresponding Type object
     * @param type llvm::Type for transformation
     * @return unique_ptr to corresponding Type object
     */
    Type* getType(const llvm::Type* type);


    /**
     * @brief getBinaryType Returns type that would be result of a binary operation
     * @param left left argument of the operation
     * @param right right argument of the operation
     * @return unique_ptr to Type object
     */
    static Type* getBinaryType(Type* left, Type* right);

    /**
     * @brief getStructName Parses LLVM struct (union) name into llvm2c struct name.
     * @param structName LLVM struct name
     * @return New struct name
     */
    static std::string getStructName(const std::string& structName);

    /**
     * @brief hasTypeDefs Returns whether the program has any typedefs.
     * @return True if program has typedefs, false otherwise
     */
    bool hasTypeDefs() const {
        return !typeDefs.empty();
    }

    IntegerType* toggleSignedness(IntegerType* ty);

    Type* pointerTo(Type* type);

    IntegerType* setSigned(IntegerType* ty);
    IntegerType* setUnsigned(IntegerType* ty);
};
