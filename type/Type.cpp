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

bool FunctionPointerType::classof(const Type* type) {
    return type->getKind() == TK_FunctionPointerType;
}

std::string FunctionPointerType::defToString() const {
    if (typeEnd.empty()) {
        return "typedef " + type + " " + name + ";";
    }

    return "typedef " + type + name + typeEnd + ";";
}

AggregateType::AggregateType(TypeKind kind, const std::string& name): Type(kind), name(name) { }

bool AggregateType::classof(const Type* ty) {
    return ty->getKind() == TK_StructType || ty->getKind() == TK_UnionType;
}

void AggregateType::addItem(Type* type, const std::string& name) {
    items.emplace_back(type, name);
}

StructType::StructType(const std::string& name)
    : AggregateType(TK_StructType, name) { }

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

UnionType::UnionType(const std::string& name) : AggregateType(TK_UnionType, name) {}

void UnionType::print() const {
    llvm::outs() << toString();
}

unsigned UnionType::indexOfType(Type* type) const {
    for (unsigned i = 0; i < items.size(); ++i) {
        if (items[i].first == type) {
            return i;
        }
    }

    assert(false && "UnionType::indexOfType: type not found in the union");
    abort();
}

std::string UnionType::toString() const {
    std::string ret = getConstStaticString();
    return ret + "union " + name;
}

bool UnionType::classof(const Type* type) {
    return type->getKind() == TK_UnionType;
}

ArrayType::ArrayType(Type* type, unsigned int size)
    : Type(TK_ArrayType),
      type(std::move(type)),
      size(size) {
    isStructArray = false;
    isPointerArray = false;

    if (auto AT = llvm::dyn_cast_or_null<ArrayType>(this->type)) {
        isStructArray = AT->isStructArray;
        structName = AT->structName;

        isPointerArray = AT->isPointerArray;
        pointer = AT->pointer;
    }

    if (auto ST = llvm::dyn_cast_or_null<StructType>(this->type)) {
        isStructArray = true;
        structName = ST->name;
    }

    if (auto PT = llvm::dyn_cast_or_null<PointerType>(this->type)) {
        isPointerArray = true;
        pointer = PT;
    }
}

ArrayType::ArrayType(const ArrayType& other): Type(TK_ArrayType) {
    size = other.size;
    type = other.type;
    isStructArray = other.isStructArray;
    structName = other.structName;
    isPointerArray = other.isPointerArray;
    pointer = other.pointer;
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
    if (ArrayType* AT = llvm::dyn_cast_or_null<ArrayType>(type)) {
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

void VoidType::print() const {
    llvm::outs() << toString();
}

std::string VoidType::toString() const {
    return "void";
}

bool VoidType::classof(const Type* type) {
    return type->getKind() == TK_VoidType;
}

PointerType::PointerType(Type* type): Type(TK_PointerType) {
    levels = 1;
    isArrayPointer = false;
    isStructPointer = false;

    if (auto PT = llvm::dyn_cast_or_null<PointerType>(type)) {
        isArrayPointer = PT->isArrayPointer;
        isStructPointer = PT->isStructPointer;
        structName = PT->structName;
        levels = PT->levels + 1;
        sizes = PT->sizes;
    }

    if (auto AT = llvm::dyn_cast_or_null<ArrayType>(type)) {
        isArrayPointer = true;
        sizes = AT->sizeToString();

        isStructPointer = AT->isStructArray;
        structName = AT->structName;
    }

    if (auto ST = llvm::dyn_cast_or_null<StructType>(type)) {
        isStructPointer = true;
        structName = ST->name;
    }

    this->type = type;
}

PointerType::PointerType(const PointerType &other): Type(TK_PointerType) {
    type = other.type;
    isArrayPointer = other.isArrayPointer;
    levels = other.levels;
    sizes = other.sizes;
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

void IntegerType::print() const {
    llvm::outs() << toString();
}

bool IntegerType::classof(const Type* type) {
    return type->getKind() == TK_IntegerType
        || type->getKind() == TK_ShortType
        || type->getKind() == TK_IntType
        || type->getKind() == TK_BoolType
        || type->getKind() == TK_CharType
        || type->getKind() == TK_LongType
        || type->getKind() == TK_LongLongType
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

bool CharType::classof(const Type* type) {
    return type->getKind() == TK_CharType;
}

IntType::IntType(bool unsignedType)
    : IntegerType("int", unsignedType, TK_IntType) { }

IntType::IntType(TypeKind kind)
    : IntegerType("int", true, kind) {
        assert(kind == TK_BoolType);
}

bool IntType::classof(const Type* type) {
    return type->getKind() == TK_IntType ||
           type->getKind() == TK_BoolType;
}

BoolType::BoolType()
    : IntType(TK_BoolType) {}

bool BoolType::classof(const Type* type) {
    return type->getKind() == TK_BoolType;
}

ShortType::ShortType(bool unsignedType)
    : IntegerType("short", unsignedType, TK_ShortType) { }

bool ShortType::classof(const Type* type) {
    return type->getKind() == TK_ShortType;
}

LongType::LongType(bool unsignedType)
    : IntegerType("long", unsignedType, TK_LongType) { }

bool LongType::classof(const Type* type) {
    return type->getKind() == TK_LongType;
}

LongLongType::LongLongType(bool unsignedType)
    : IntegerType("long long", unsignedType, TK_LongType) { }

bool LongLongType::classof(const Type* type) {
    return type->getKind() == TK_LongLongType;
}

Int128::Int128()
    : IntegerType("__int128", true, TK_Int128) { }

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

bool FloatType::classof(const Type* type) {
    return type->getKind() == TK_FloatType;
}

DoubleType::DoubleType()
    : FloatingPointType("double", TK_DoubleType) { }

bool DoubleType::classof(const Type* type) {
    return type->getKind() == TK_DoubleType;
}

LongDoubleType::LongDoubleType()
    : FloatingPointType("long double", TK_LongDoubleType) { }

bool LongDoubleType::classof(const Type* type) {
    return type->getKind() == TK_LongDoubleType;
}
