#include "Type.h"

#include "llvm/IR/DerivedTypes.h"
#include "llvm/Support/raw_ostream.h"

FunctionPointerType::FunctionPointerType(const std::string& type, const std::string& name, const std::string& typeEnd)
    : Type(TK_FunctionPointerType),
      type(type),
      name(name),
      typeEnd(typeEnd) { }

FunctionPointerType::FunctionPointerType(const FunctionPointerType& other)
    : Type(TK_FunctionPointerType) {
    type = other.type;
    name = other.name;
    typeEnd = other.typeEnd;
}

void FunctionPointerType::print() const {
    llvm::outs() << toString();
}

std::string FunctionPointerType::toString() const {
    return name;
}

std::unique_ptr<Type> FunctionPointerType::clone() const {
    return std::make_unique<FunctionPointerType>(type, name, typeEnd);
}

bool FunctionPointerType::classof(const Type* type) {
    return type->getKind() == TK_FunctionPointerType;
}

std::string FunctionPointerType::defToString() const {
    if (typeEnd.empty()) {
        return "typedef " + type + " " + name + ";";
    }

    return "typedef " + type + name + typeEnd + ";";
}

StructType::StructType(const std::string& name)
    : Type(TK_StructType), name(name) { }

StructType::StructType(const StructType& other) : Type(TK_StructType) {
    name = other.name;
}

std::unique_ptr<Type> StructType::clone() const  {
    return std::make_unique<StructType>(*this);
}

void StructType::print() const {
    llvm::outs() << toString();
}

bool StructType::classof(const Type* type) {
    return type->getKind() == TK_StructType;
}

std::string StructType::toString() const {
    std::string ret = getConstStaticString();

    return ret + "struct " + name;
}

ArrayType::ArrayType(std::unique_ptr<Type> type, unsigned int size)
    : Type(TK_ArrayType),
      type(std::move(type)),
      size(size) {
    isStructArray = false;
    isPointerArray = false;

    if (auto AT = llvm::dyn_cast_or_null<ArrayType>(this->type.get())) {
        isStructArray = AT->isStructArray;
        structName = AT->structName;

        isPointerArray = AT->isPointerArray;
        pointer = AT->pointer;
    }

    if (auto ST = llvm::dyn_cast_or_null<StructType>(this->type.get())) {
        isStructArray = true;
        structName = ST->name;
    }

    if (auto PT = llvm::dyn_cast_or_null<PointerType>(this->type.get())) {
        isPointerArray = true;
        pointer = PT;
    }
}

ArrayType::ArrayType(const ArrayType& other): Type(TK_ArrayType) {
    size = other.size;
    type = other.type->clone();
    isStructArray = other.isStructArray;
    structName = other.structName;
    isPointerArray = other.isPointerArray;
    pointer = other.pointer;
}

std::unique_ptr<Type> ArrayType::clone() const  {
    return std::make_unique<ArrayType>(*this);
}

void ArrayType::print() const {
    type->print();
}

void ArrayType::printSize() const {
    llvm::outs() << sizeToString();
}

bool ArrayType::classof(const Type* type) {
    return type->getKind() == TK_ArrayType;
}

std::string ArrayType::toString() const {
    std::string ret = getConstStaticString();

    return ret + type->toString();
}

std::string ArrayType::sizeToString() const {
    std::string ret;

    ret += "[";
    ret += std::to_string(size);
    ret += "]";
    if (ArrayType* AT = llvm::dyn_cast_or_null<ArrayType>(type.get())) {
        ret += AT->sizeToString();
    }

    return ret;
}

std::string ArrayType::surroundName(const std::string& name) {
    std::string ret;
    if (isPointerArray && pointer->isArrayPointer) {
        ret = "(";
        for (unsigned i = 0; i < pointer->levels; i++) {
            ret += "*";
        }
        return ret + name + sizeToString() + ")" + pointer->sizes;
    } else {
        return name + sizeToString();
    }
}

VoidType::VoidType(): Type(TK_VoidType) {}

std::unique_ptr<Type> VoidType::clone() const  {
    return std::make_unique<VoidType>();
}

void VoidType::print() const {
    llvm::outs() << toString();
}

std::string VoidType::toString() const {
    return "void";
}

bool VoidType::classof(const Type* type) {
    return type->getKind() == TK_VoidType;
}

PointerType::PointerType(std::unique_ptr<Type> type): Type(TK_PointerType) {
    levels = 1;
    isArrayPointer = false;
    isStructPointer = false;

    if (auto PT = llvm::dyn_cast_or_null<PointerType>(type.get())) {
        isArrayPointer = PT->isArrayPointer;
        isStructPointer = PT->isStructPointer;
        structName = PT->structName;
        levels = PT->levels + 1;
        sizes = PT->sizes;
    }

    if (auto AT = llvm::dyn_cast_or_null<ArrayType>(type.get())) {
        isArrayPointer = true;
        sizes = AT->sizeToString();

        isStructPointer = AT->isStructArray;
        structName = AT->structName;
    }

    if (auto ST = llvm::dyn_cast_or_null<StructType>(type.get())) {
        isStructPointer = true;
        structName = ST->name;
    }

    this->type = type->clone();
}

PointerType::PointerType(const PointerType &other): Type(TK_PointerType) {
    type = other.type->clone();
    isArrayPointer = other.isArrayPointer;
    levels = other.levels;
    sizes = other.sizes;
}

std::unique_ptr<Type> PointerType::clone() const  {
    return std::make_unique<PointerType>(*this);
}

void PointerType::print() const {
    llvm::outs() << toString();
}

bool PointerType::classof(const Type* type) {
    return type->getKind() == TK_PointerType;
}

std::string PointerType::toString() const {
    std::string ret = getConstStaticString();

    if (isArrayPointer) {
        return ret + type->toString();
    }

    return ret + type->toString() + "*";
}

std::string PointerType::surroundName(const std::string& name) {
    std::string ret;

    if (isArrayPointer && name != "0") {
        ret = "(";
        for (unsigned i = 0; i < levels; i++) {
            ret += "*";
        }
        ret += name + ")";
    }

    if (isArrayPointer) {
        ret = ret + sizes;
    }

    if (!ret.empty())
        return ret;

    return name;
}

IntegerType::IntegerType(const std::string& name, bool unsignedType)
    : Type(TK_IntegerType),
      name(name),
      unsignedType(unsignedType) { }

IntegerType::IntegerType(const std::string& name, bool unsignedType, TypeKind kind)
    : Type(kind),
      name(name),
      unsignedType(unsignedType) { }


IntegerType::IntegerType(const IntegerType& other): Type(other.getKind()) {
    name = other.name;
    unsignedType = other.unsignedType;
}

std::unique_ptr<Type> IntegerType::clone() const  {
    return std::make_unique<IntegerType>(*this);
}

void IntegerType::print() const {
    llvm::outs() << toString();
}

bool IntegerType::classof(const Type* type) {
    return type->getKind() == TK_IntegerType
        || type->getKind() == TK_ShortType
        || type->getKind() == TK_IntType
        || type->getKind() == TK_CharType
        || type->getKind() == TK_LongType
        || type->getKind() == TK_Int128;
}

std::string IntegerType::toString() const {
    std::string ret = getConstStaticString();

    if (unsignedType) {
        ret += "unsigned ";
    }
    ret += name;

    return ret;
}

CharType::CharType(bool unsignedType)
    : IntegerType("char", unsignedType, TK_CharType) { }

std::unique_ptr<Type> CharType::clone() const  {
    return std::make_unique<CharType>(*this);
}

bool CharType::classof(const Type* type) {
    return type->getKind() == TK_CharType;
}

IntType::IntType(bool unsignedType)
    : IntegerType("int", unsignedType, TK_IntType) { }

std::unique_ptr<Type> IntType::clone() const  {
    return std::make_unique<IntType>(*this);
}

bool IntType::classof(const Type* type) {
    return type->getKind() == TK_IntType;
}

ShortType::ShortType(bool unsignedType)
    : IntegerType("short", unsignedType, TK_ShortType) { }

std::unique_ptr<Type> ShortType::clone() const  {
    return std::make_unique<ShortType>(*this);
}

bool ShortType::classof(const Type* type) {
    return type->getKind() == TK_ShortType;
}

LongType::LongType(bool unsignedType)
    : IntegerType("long", unsignedType, TK_LongType) { }

std::unique_ptr<Type> LongType::clone() const  {
    return std::make_unique<LongType>(*this);
}

bool LongType::classof(const Type* type) {
    return type->getKind() == TK_LongType;
}

Int128::Int128()
    : IntegerType("__int128", true, TK_Int128) { }

std::unique_ptr<Type> Int128::clone() const {
    return std::make_unique<Int128>();
}

bool Int128::classof(const Type* type) {
    return type->getKind() == TK_Int128;
}

FloatingPointType::FloatingPointType(const std::string& name)
    : Type(TK_FloatingPointType), name(name) { }

FloatingPointType::FloatingPointType(const std::string& name, TypeKind kind)
    : Type(kind), name(name) { }

FloatingPointType::FloatingPointType(const FloatingPointType& other): Type(other.getKind()) {
    name = other.name;
}

std::unique_ptr<Type> FloatingPointType::clone() const  {
    return std::make_unique<FloatingPointType>(*this);
}

bool FloatingPointType::classof(const Type* type) {
    return type->getKind() == TK_FloatingPointType
        || type->getKind() == TK_FloatType
        || type->getKind() == TK_DoubleType
        || type->getKind() == TK_LongDoubleType;
}

void FloatingPointType::print() const {
    llvm::outs() << toString();
}

std::string FloatingPointType::toString() const {
    std::string ret = getConstStaticString();

    return ret + name;
}

FloatType::FloatType()
    : FloatingPointType("float", TK_FloatType) { }

std::unique_ptr<Type> FloatType::clone() const  {
    return std::make_unique<FloatType>(*this);
}

bool FloatType::classof(const Type* type) {
    return type->getKind() == TK_FloatType;
}

DoubleType::DoubleType()
    : FloatingPointType("double", TK_DoubleType) { }

std::unique_ptr<Type> DoubleType::clone() const  {
    return std::make_unique<DoubleType>(*this);
}

bool DoubleType::classof(const Type* type) {
    return type->getKind() == TK_DoubleType;
}

LongDoubleType::LongDoubleType()
    : FloatingPointType("long double", TK_LongDoubleType) { }

std::unique_ptr<Type> LongDoubleType::clone() const  {
    return std::make_unique<LongDoubleType>(*this);
}

bool LongDoubleType::classof(const Type* type) {
    return type->getKind() == TK_LongDoubleType;
}
