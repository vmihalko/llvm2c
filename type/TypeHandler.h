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
    Program* program;
    llvm::DenseMap<const llvm::Type*, std::unique_ptr<Type>> typeDefs; //map containing typedefs
    std::unordered_map<const llvm::Type*, std::unique_ptr<Type>> typeCache;

    unsigned typeDefCount = 0; //variable used for creating new name for typedef

    /**
     * @brief getTypeDefName Creates new name for a typedef.
     * @return String containing new name for a typedef
     */
    std::string getTypeDefName() {
        std::string ret = "typeDef_" + std::to_string(typeDefCount);
        typeDefCount++;
        return ret;
    }

    template<typename T, typename ...Args>
    Type* makeCachedType(const llvm::Type* ty, Args&&... args) {
        auto ptr = std::make_unique<T>(std::forward<Args>(args)...);
        auto* result = ptr.get();
        typeCache[ty] = std::move(ptr);
        return result;
    }

public:
    std::vector<const FunctionPointerType*> sortedTypeDefs; //vector of sorted typedefs, used in output

    TypeHandler(Program* program)
        : program(program) { }

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
};
