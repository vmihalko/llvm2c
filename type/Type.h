#pragma once

#include "llvm/IR/Type.h"

#include <string>
#include <memory>

/**
 * @brief The Type class is an abstract class for all types.
 */
class Type {
public:
    enum TypeKind {
        TK_FunctionPointerType,
        TK_StructType,
        TK_UnionType,
        TK_PointerType,
        TK_ArrayType,
        TK_VoidType,
        TK_IntegerType,
        TK_CharType,
        TK_IntType,
        TK_BoolType, // alias for IntType that we use in logical exprs
        TK_ShortType,
        TK_LongType,
        TK_Int128,
        TK_FloatingPointType,
        TK_FloatType,
        TK_DoubleType,
        TK_LongDoubleType,
    };
private:
    const TypeKind kind;
public:

    Type(TypeKind kind): kind(kind) {}
    virtual ~Type() = default;
    virtual void print() const = 0;
    virtual std::string toString() const = 0;

    bool isConst = false;

    std::string getConstStaticString() const {
        std::string ret;

        if (isConst) {
            ret += "const ";
        }

        return ret;
    }

    virtual std::string surroundName(const std::string& name) {
        return name;
    }

    TypeKind getKind() const {
        return kind;
    }

    static bool classof(const Type* type);
};

/**
 * @brief The FunctionPointerType class represents function pointer.
 * It contains all the information needed for printing the FunctionPointerType definition.
 */
class FunctionPointerType : public Type {
friend class TypeHandler;
public:
    //type is split into two string so the name can be printed separately
    std::string type;
    std::string name;
    std::string typeEnd;

public:
    FunctionPointerType(const std::string&, const std::string&, const std::string&);
    FunctionPointerType(const FunctionPointerType&);

    void print() const override;
    std::string toString() const override;

    /**
     * @brief defToString Returns definition of FunctionPointerType as a typedef string.
     * @return String with FunctionPointerType definition
     */
    std::string defToString() const;

    static bool classof(const Type* type);
};

class AggregateType : public Type {
public:
    std::string name;
    std::vector<std::pair<Type*, std::string>> items; //elements of the struct

    AggregateType(TypeKind kind, const std::string& name);

    void addItem(Type* type, const std::string& name);

    static bool classof(const Type* type);

    virtual ~AggregateType() = default;
};

/**
 * @brief The StructType class represents struct as a type.
 */
class StructType : public AggregateType {
public:
    StructType(const std::string&);
    StructType(const StructType&);

    void print() const override;
    std::string toString() const override;

    static bool classof(const Type* type);
};

/**
 * @brief The UnionType class represents union as a type.
 */
class UnionType : public AggregateType {
public:
    UnionType(const std::string&);

    void print() const override;
    std::string toString() const override;

    unsigned indexOfType(Type* type) const;

    static bool classof(const Type* type);
};


/**
 * @brief The PointerType class represents pointer.
 */
class PointerType : public Type {
public:
    Type* type;
    unsigned levels; //number of pointers (for instance int** is level 2), used for easier printing

    bool isArrayPointer; //indicates whether the pointer is pointing to array
    std::string sizes; //sizes of arrays

    bool isStructPointer; //indicates whether the pointer is pointing to struct
    std::string structName; //name of the struct

    PointerType(Type*);
    PointerType(const PointerType& other);

    void print() const override;
    std::string toString() const override;

    std::string surroundName(const std::string& name) override;

    static bool classof(const Type* type);
};

/**
 * @brief The ArrayType class represents array.
 */
class ArrayType : public Type {
public:
    Type* type;
    unsigned int size;

    bool isStructArray; //indicates whether the array contains structs
    std::string structName; //name of the structs

    bool isPointerArray; //indicates whether the array contains pointers
    PointerType* pointer; //pointers contained in array

    ArrayType(Type*, unsigned int);
    ArrayType(const ArrayType&);

    void print() const override;
    std::string toString() const override;

    void printSize() const;
    std::string sizeToString() const;

    std::string surroundName(const std::string& name) override;

    // ArrayType putInside(const ArrayType&);

    static bool classof(const Type* type);
};

/**
 * @brief The VoidType class represents void.
 */
class VoidType : public Type {
public:
    VoidType();
    void print() const override;
    std::string toString() const override;

    static bool classof(const Type* type);
};

/**
 * @brief The IntegerType class is a base class for all integer types.
 */
class IntegerType : public Type {
private:
    std::string name;

public:
    bool unsignedType;

    IntegerType(const std::string&, bool);
    IntegerType(const std::string&, bool, TypeKind);
    IntegerType(const IntegerType&);

    void print() const override;
    std::string toString() const override;

    static bool classof(const Type* type);
};

/**
 * @brief The CharType class represents char.
 */
class CharType : public IntegerType {
public:
    CharType(bool);


    static bool classof(const Type* type);
};

/**
 * @brief The IntType class represents int.
 */
class IntType : public IntegerType {
public:
    IntType(bool);
protected:
    IntType(TypeKind); // ctor for BoolType

    static bool classof(const Type* type);
};

/**
 * @brief The ShortType class represents short.
 */
class ShortType : public IntegerType {
public:
    ShortType(bool);


    static bool classof(const Type* type);
};

/**
 * @brief The LongType class represents long.
 */
class LongType : public IntegerType {
public:
    LongType(bool);


    static bool classof(const Type* type);
};

/**
 * @brief The Int128 class represents __int128.
 */
class Int128 : public IntegerType {
public:
    Int128();


    static bool classof(const Type* type);
};

/**
 * @brief The IntType class represents int.
 */
class BoolType : public IntType {
public:
    BoolType();

    static bool classof(const Type* type);
};


/**
 * @brief The FloatingPointType class is a base class for all floating point types.
 */
class FloatingPointType : public Type {
private:
    std::string name;

public:
    FloatingPointType(const std::string&);
    FloatingPointType(const std::string&, TypeKind kind);
    FloatingPointType(const FloatingPointType&);

    void print() const override;
    std::string toString() const override;

    static bool classof(const Type* type);
};

/**
 * @brief The FloatType class represents float.
 */
class FloatType : public FloatingPointType {
public:
    FloatType();


    static bool classof(const Type* type);
};

/**
 * @brief The DoubleType class represents double.
 */
class DoubleType : public FloatingPointType {
public:
    DoubleType();


    static bool classof(const Type* type);
};

/**
 * @brief The LongDoubleType class represents long double.
 */
class LongDoubleType : public FloatingPointType {
public:
    LongDoubleType();


    static bool classof(const Type* type);
};
