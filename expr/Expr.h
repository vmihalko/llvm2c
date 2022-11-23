#pragma once

#include <string>
#include <map>
#include <vector>
#include <memory>

#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/Casting.h"

#include "../type/Type.h"
#include "ExprVisitor.h"

class Block;

/**
 * @brief The Expr class is an abstract class for all expressions.
 */
class Expr {
public:
    enum ExprKind {
        EK_AggregateElement,
        EK_ArrayElement,
        EK_ExtractValueExpr,
        EK_Value,
        EK_GlobalValue,
        EK_IfExpr,
        EK_GotoExpr,
        EK_LatchExpr,
        EK_SwitchExpr,
        EK_AsmExpr,
        EK_CallExpr,
        EK_PointerShift,
        EK_GepExpr,
        EK_SelectExpr,
        EK_StackAlloc,

        EK_RefExpr,
        EK_DerefExpr,
        EK_RetExpr,
        EK_CastExpr,

        EK_AddExpr,
        EK_SubExpr,
        EK_AssignExpr,
        EK_MulExpr,
        EK_DivExpr,
        EK_RemExpr,
        EK_AndExpr,
        EK_OrExpr,
        EK_XorExpr,
        EK_CmpExpr,
        EK_AshrExpr,
        EK_LshrExpr,
        EK_ShlExpr,
        EK_AggregateInitializer,
        EK_ArrowExpr,
        EK_LogicalAnd,
        EK_LogicalOr,
        EK_LogicalNot,

        EK_ExprList,
        EK_MinusExpr,
        EK_DoWhile,
    };
private:
    const ExprKind kind;
public:
    ExprKind getKind() const { return kind; }
    Expr(ExprKind kind): kind(kind) {}

    virtual ~Expr() = default;
    virtual void accept(ExprVisitor& visitor) = 0;
    virtual const Type* getType() const = 0;
    virtual Type* getType() = 0;
    virtual void setType(Type*) = 0;
    virtual bool isZero() const = 0;
    virtual bool isSimple() const = 0;
};

/**
 * @brief The ExprBase class is a base class for all expressions. It contains type of the expression and function to get/set the type.
 */
class ExprBase : public Expr {
private:
    Type* type;

public:
    ExprBase(ExprKind kind) : Expr(kind) {}

    const Type* getType() const override {
        return type;
    }

    Type* getType() override {
        return type;
    }

    void setType(Type* type) override {
        this->type = type;
    }

    bool isZero() const override {
        return false;
    }

    bool isSimple() const override {
        return false;
    }
};

class ExprList : public ExprBase {
public:
    std::vector<Expr*> expressions;

    ExprList(std::vector<Expr*>&& expressions);

    void accept(ExprVisitor& visitor) override;

    static bool classof(const Expr* expr);
};

/**
 * @brief The StructElement class represents access to element of a struct.
 */
class AggregateElement : public ExprBase {
public:
    Expr* expr; //expression being accessed
    unsigned element; //number of the element

    AggregateElement(Expr*, unsigned);

    void accept(ExprVisitor& visitor) override;

    static bool classof(const Expr* expr);
};

/**
 * @brief The ArrayElement class represents access to element of an array.
 */
class ArrayElement : public ExprBase {
public:
    Expr* expr; //expression being accessed
    Expr* element; //expression representing index of the element

    ArrayElement(Expr*, Expr*);
    ArrayElement(Expr*, Expr*, Type*);

    void accept(ExprVisitor& visitor) override;

    static bool classof(const Expr* expr);
};

/**
 * @brief The ExtractValueExpr class represents extractvalue instruction in C. It is made of sequence of StructElement and ArrayElements expressions.
 */
class ExtractValueExpr : public ExprBase {
public:
    std::vector<std::unique_ptr<Expr>> indices; //sequence of StructElement and ArrayElements expressions

    ExtractValueExpr(std::vector<std::unique_ptr<Expr>>&&);

    void accept(ExprVisitor& visitor) override;

    static bool classof(const Expr* expr);
};

/**
 * @brief The Value class represents variable or constant value.
 */
class Value : public ExprBase {
public:
    std::string valueName;

    Value(const std::string&, Type*);
    Value(const std::string&, Type*, ExprKind kind);

    void accept(ExprVisitor& visitor) override;

    bool isZero() const override;

    bool isSimple() const override;

    void setName(const std::string& name) { valueName = name; }

    static bool classof(const Expr* expr);
};

/**
 * @brief The GlobalValue class represents global variable.
 */
class GlobalValue : public Value {
public:
    Expr* value;
    bool isStatic = false;

    GlobalValue(const std::string&, Expr*, Type*);

    void accept(ExprVisitor& visitor) override;

    bool isSimple() const override;

    static bool classof(const Expr* expr);
};

/**
 * @brief The IfExpr class represents br instruction in C as an if-else statement.
 */
class IfExpr : public ExprBase {
public:
    Expr* cmp; //expression used as a condition
    Expr* trueList;
    Expr* falseList;

    IfExpr(Expr*, Expr*, Expr*);

    void accept(ExprVisitor& visitor) override;

    static bool classof(const Expr* expr);
};

/**
 * @brief The GotoExpr class represents br instruction in C as a goto statement.
 */
class GotoExpr : public ExprBase {
public:
    Block* target; //else goto falseBlock

    GotoExpr(Block* target);

    void accept(ExprVisitor& visitor) override;

    static bool classof(const Expr* expr);
};

/**
 * @brief The LatchExpr class represents artifical `latch:` label for a DoWhile expr.
 */
class LatchExpr : public ExprBase {
public:
    Block* target; //else goto falseBlock

    LatchExpr(Block* target);

    void accept(ExprVisitor& visitor) override;

    static bool classof(const Expr* expr);
};


/**
 * @brief The SwitchExpr class represents switch.
 */
class SwitchExpr : public ExprBase {
public:
    Expr* cmp; //expression used in switch
    Expr* def; //default
    std::map<int, Expr*> cases; //cases of switch

    SwitchExpr(Expr*, Expr*, std::map<int, Expr*>);

    void accept(ExprVisitor& visitor) override;

    static bool classof(const Expr* expr);
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

    static bool classof(const Expr* expr);
};

/**
 * @brief The CallExpr class represents function call.
 */
class CallExpr : public ExprBase {
public:
    std::string funcName; //name of the called function
    std::vector<Expr*> params; //parameters of the function call
    Expr* funcValue; //expression in case of calling function pointer

    CallExpr(Expr*, const std::string&, std::vector<Expr*>, Type*);

    void accept(ExprVisitor& visitor) override;

    static bool classof(const Expr* expr);

    bool isSimple() const override;
};

/**
 * @brief The PointerShift class represents shifting a pointer.
 */
class PointerShift : public ExprBase {
public:
    Type* ptrType; //type of the pointer
    Expr* pointer; //expression being shifted
    Expr* move; //expression representing number used for shifting

    PointerShift(Type*, Expr*, Expr*);

    void accept(ExprVisitor& visitor) override;

    static bool classof(const Expr* expr);
};

/**
 * @brief The GepExpr class represents getelementptr instruction in C. It is made of sequence of StructElement, ArrayElement and PointerShift expressions.
 */
class GepExpr : public ExprBase {
public:
    std::vector<Expr*> indices; //sequence of StructElement, ArrayElement and PointerShift expressions
    GepExpr(std::vector<Expr*>&&);

    void accept(ExprVisitor& visitor) override;

    static bool classof(const Expr* expr);
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

    static bool classof(const Expr* expr);
};

class StackAlloc : public ExprBase {
public:
    Value* value;

    StackAlloc(Value*);

    void accept(ExprVisitor& visitor) override;

    static bool classof(const Expr* expr);
};

class AggregateInitializer : public ExprBase {
public:
    std::vector<Expr *> values;

    AggregateInitializer(std::vector<Expr *> values);

    void accept(ExprVisitor& visitor) override;

    static bool classof(const Expr* expr);
};

/**
 * @brief The ArrowExpr represents an access to structure field via structure pointer
 * struct->field
 */
class ArrowExpr : public ExprBase {
public:
    Expr* expr; //expression being accessed
    unsigned element; //number of the element

    ArrowExpr(Expr*, unsigned);

    void accept(ExprVisitor& visitor) override;

    static bool classof(const Expr* expr);
};

class LogicalAnd : public ExprBase {
public:
    Expr* lhs;
    Expr* rhs;

    LogicalAnd(Expr*, Expr*);

    void accept(ExprVisitor& visitor) override;

    static bool classof(const Expr* expr);
};

class LogicalOr : public ExprBase {
public:
    Expr* lhs;
    Expr* rhs;

    LogicalOr(Expr*, Expr*);

    void accept(ExprVisitor& visitor) override;

    static bool classof(const Expr* expr);
};

class DoWhile : public ExprBase {
public:
    Expr* body;
    Expr* cond;

    DoWhile(Expr* body, Expr* cond);

    void accept(ExprVisitor& visitor) override;

    static bool classof(const Expr* expr);
};
