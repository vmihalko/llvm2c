#pragma once

#include <string>
#include <map>
#include <vector>
#include <memory>

#include "llvm/Support/raw_ostream.h"

#include "../type/Type.h"
#include "ExprVisitor.h"

/**
 * @brief The Expr class is an abstract class for all expressions.
 */
class Expr {
public:
    virtual ~Expr() = default;
    virtual void accept(ExprVisitor& visitor) = 0;
    virtual const Type* getType() const = 0;
    virtual Type* getType() = 0;
    virtual void setType(std::unique_ptr<Type>) = 0;
    virtual bool isZero() const = 0;
};

/**
 * @brief The ExprBase class is a base class for all expressions. It contains type of the expression and function to get/set the type.
 */
class ExprBase : public Expr {
private:
    std::unique_ptr<Type> type;

public:
    const Type* getType() const override {
        return type.get();
    }

    Type* getType() override {
        return type.get();
    }

    void setType(std::unique_ptr<Type> type) override {
        this->type = std::move(type);
    }

    bool isZero() const override {
        return false;
    }
};

/**
 * @brief The Struct class represents struct with all the information needed for using it.
 */
class Struct : public ExprBase {
public:
    std::string name;
    std::vector<std::pair<std::unique_ptr<Type>, std::string>> items; //elements of the struct

    Struct(const std::string&);

    /**
     * @brief addItem Adds new struct element to the vector items.
     * @param type Type of the element
     * @param name Name of the element
     */
    void addItem(std::unique_ptr<Type> type, const std::string& name);

    void accept(ExprVisitor& visitor) override;
};

/**
 * @brief The StructElement class represents access to element of a struct.
 */
class StructElement : public ExprBase {
public:
    Struct* strct; //struct being accessed
    Expr* expr; //expression being accessed
    unsigned element; //number of the element

    StructElement(Struct*, Expr*, unsigned);

    void accept(ExprVisitor& visitor) override;
};

/**
 * @brief The ArrayElement class represents access to element of an array.
 */
class ArrayElement : public ExprBase {
public:
    Expr* expr; //expression being accessed
    Expr* element; //expression representing index of the element

    ArrayElement(Expr*, Expr*);
    ArrayElement(Expr*, Expr*, std::unique_ptr<Type>);

    void accept(ExprVisitor& visitor) override;
};

/**
 * @brief The ExtractValueExpr class represents extractvalue instruction in C. It is made of sequence of StructElement and ArrayElements expressions.
 */
class ExtractValueExpr : public ExprBase {
public:
    std::vector<std::unique_ptr<Expr>> indices; //sequence of StructElement and ArrayElements expressions

    ExtractValueExpr(std::vector<std::unique_ptr<Expr>>&);

    void accept(ExprVisitor& visitor) override;
};

/**
 * @brief The Value class represents variable or constant value.
 */
class Value : public ExprBase {
public:
    std::string valueName;

    Value(const std::string&, std::unique_ptr<Type>);

    void accept(ExprVisitor& visitor) override;

    bool isZero() const override;
};

/**
 * @brief The GlobalValue class represents global variable.
 */
class GlobalValue : public Value {
public:
    std::string value;

    GlobalValue(const std::string&, const std::string&, std::unique_ptr<Type>);

    bool isDefined = false;

    void accept(ExprVisitor& visitor) override;
};

/**
 * @brief The IfExpr class represents br instruction in C as an if-else statement.
 */
class IfExpr : public ExprBase {
public:
    Expr* cmp; //expression used as a condition
    std::string trueBlock; //goto trueBlock if condition is true
    std::string falseBlock; //else goto falseBlock

    IfExpr(Expr*, const std::string&, const std::string&);
    IfExpr(const std::string& trueBlock);

    void accept(ExprVisitor& visitor) override;
};

/**
 * @brief The SwitchExpr class represents switch.
 */
class SwitchExpr : public ExprBase {
public:
    Expr* cmp; //expression used in switch
    std::string def; //default
    std::map<int, std::string> cases; //cases of switch

    SwitchExpr(Expr*, const std::string&, std::map<int, std::string>);

    void accept(ExprVisitor& visitor) override;
};

/**
 * @brief The AsmExpr class represents calling of inline asm.
 */
class AsmExpr : public ExprBase {
public:
    std::string inst; //asm string
    std::vector<std::pair<std::string, Expr*>> output; //output constraints
    std::vector<std::pair<std::string, Expr*>> input; //input constraints
    std::string clobbers; //clobber

    AsmExpr(const std::string&, const std::vector<std::pair<std::string, Expr*>>&, const std::vector<std::pair<std::string, Expr*>>&, const std::string&);

    /**
     * @brief addOutputExpr Adds Expr to the output vector
     * @param expr Output expression
     * @param pos Position in vector
     */
    void addOutputExpr(Expr* expr, unsigned pos);

    void accept(ExprVisitor& visitor) override;
};

/**
 * @brief The CallExpr class represents function call.
 */
class CallExpr : public ExprBase {
public:
    std::string funcName; //name of the called function
    std::vector<Expr*> params; //parameters of the function call
    Expr* funcValue; //expression in case of calling function pointer

    CallExpr(Expr*, const std::string&, std::vector<Expr*>, std::unique_ptr<Type>);

    void accept(ExprVisitor& visitor) override;
};

/**
 * @brief The PointerShift class represents shifting a pointer.
 */
class PointerShift : public ExprBase {
public:
    std::unique_ptr<Type> ptrType; //type of the pointer
    Expr* pointer; //expression being shifted
    Expr* move; //expression representing number used for shifting

    PointerShift(std::unique_ptr<Type>, Expr*, Expr*);

    void accept(ExprVisitor& visitor) override;
};

/**
 * @brief The GepExpr class represents getelementptr instruction in C. It is made of sequence of StructElement, ArrayElement and PointerShift expressions.
 */
class GepExpr : public ExprBase {
public:
    std::vector<std::unique_ptr<Expr>> indices; //sequence of StructElement, ArrayElement and PointerShift expressions
    GepExpr(std::vector<std::unique_ptr<Expr>>&);

    void accept(ExprVisitor& visitor) override;
};

/**
 * @brief The SelectExpr class represents select instruction in C (comp ? left : right).
 */
class SelectExpr : public ExprBase {
public:
    Expr* left;
    Expr* right;
    Expr* comp;

    SelectExpr(Expr*, Expr*, Expr*);

    void accept(ExprVisitor& visitor) override;
};

class StackAlloc : public ExprBase {
public:
    Value* value;

    StackAlloc(Value*);

    void accept(ExprVisitor& visitor) override;

    const Type* getType() const override;
    Type* getType() override;
};
