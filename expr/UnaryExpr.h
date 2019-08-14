#pragma once

#include <string>

#include "Expr.h"

/**
 * @brief The UnaryExpr class is a base class for all unary expressions.
 */
class UnaryExpr : public ExprBase {
public:
    UnaryExpr(Expr *, ExprKind);

    Expr* expr; //operand of unary operation
};

/**
 * @brief The RefExpr class represent reference.
 */
class RefExpr : public UnaryExpr {
public:
    RefExpr(Expr*);

    void accept(ExprVisitor& visitor) override;

    static bool classof(const Expr* expr);
};

/**
 * @brief The DerefExpr class represents dereference.
 */
class DerefExpr : public UnaryExpr {
public:
    DerefExpr(Expr*);

    void accept(ExprVisitor& visitor) override;

    static bool classof(const Expr* expr);
};

/**
 * @brief The RetExpr class represents return.
 */
class RetExpr : public UnaryExpr {
public:
    RetExpr(Expr*);
    RetExpr();

    void accept(ExprVisitor& visitor) override;

    static bool classof(const Expr* expr);
};

/**
 * @brief The CastExpr class represents cast.
 */
class CastExpr : public UnaryExpr {
public:
    CastExpr(Expr*, std::unique_ptr<Type>);

    void accept(ExprVisitor& visitor) override;

    static bool classof(const Expr* expr);
};

class MinusExpr : public UnaryExpr {
public:
    MinusExpr(Expr*);

    void accept(ExprVisitor& visitor) override;

    static bool classof(const Expr* expr);
};
