#include "UnaryExpr.h"

#include "llvm/Support/raw_ostream.h"
#include "../type/TypeHandler.h"

/*
 * UnaryExpr classes
 */

UnaryExpr::UnaryExpr(Expr *expr, ExprKind kind): ExprBase(kind) {
    this->expr = expr;
    if (expr) {
        setType(expr->getType());
    }
}

RefExpr::RefExpr(Expr* expr, Type* type) :
    UnaryExpr(expr, EK_RefExpr) {
        _expr = expr;
    setType(type);
}

void RefExpr::accept(ExprVisitor& visitor) {
    visitor.visit(*this);
}

bool RefExpr::classof(const Expr* expr) {
    return expr->getKind() == EK_RefExpr;
}

DerefExpr::DerefExpr(Expr* expr) :
    UnaryExpr(expr, EK_DerefExpr) {
    if (auto PT = llvm::dyn_cast_or_null<PointerType>(expr->getType())) {
        setType(PT->type);
    }
}

void DerefExpr::accept(ExprVisitor& visitor) {
    visitor.visit(*this);
}

bool DerefExpr::classof(const Expr* expr) {
    return expr->getKind() == EK_DerefExpr;
}

RetExpr::RetExpr(Expr* ret)
    : UnaryExpr(ret, EK_RetExpr) { }

RetExpr::RetExpr()
    : UnaryExpr(nullptr, EK_RetExpr) { }

void RetExpr::accept(ExprVisitor& visitor) {
    visitor.visit(*this);
}

bool RetExpr::classof(const Expr* expr) {
    return expr->getKind() == EK_RetExpr;
}

CastExpr::CastExpr(Expr* expr, Type* type)
    : UnaryExpr(expr, EK_CastExpr) {
    setType(type);
}

void CastExpr::accept(ExprVisitor& visitor) {
    visitor.visit(*this);
}

bool CastExpr::classof(const Expr* expr) {
    return expr->getKind() == EK_CastExpr;
}

MinusExpr::MinusExpr(Expr* expr) : UnaryExpr(expr, EK_MinusExpr) {
    setType(expr->getType());
}

void MinusExpr::accept(ExprVisitor& visitor) {
    visitor.visit(*this);
}

bool MinusExpr::classof(const Expr* expr) {
    return expr->getKind() == EK_MinusExpr;
}

LogicalNot::LogicalNot(Expr* expr) : UnaryExpr(expr, EK_LogicalNot) {
    setType(expr->getType());
}

void LogicalNot::accept(ExprVisitor& visitor) {
    visitor.visit(*this);
}

bool LogicalNot::classof(const Expr* expr) {
    return expr->getKind() == EK_LogicalNot;
}
