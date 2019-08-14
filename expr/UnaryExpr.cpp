#include "UnaryExpr.h"

#include "llvm/Support/raw_ostream.h"

/*
 * UnaryExpr classes
 */

UnaryExpr::UnaryExpr(Expr *expr, ExprKind kind): ExprBase(kind) {
    this->expr = expr;
    if (expr) {
        setType(expr->getType()->clone());
    }
}

RefExpr::RefExpr(Expr* expr) :
    UnaryExpr(expr, EK_RefExpr) {
    setType(std::make_unique<PointerType>(expr->getType()->clone()));
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
        setType(PT->type->clone());
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

CastExpr::CastExpr(Expr* expr, std::unique_ptr<Type> type)
    : UnaryExpr(expr, EK_CastExpr) {
    setType(std::move(type));
}

void CastExpr::accept(ExprVisitor& visitor) {
    visitor.visit(*this);
}

bool CastExpr::classof(const Expr* expr) {
    return expr->getKind() == EK_CastExpr;
}

MinusExpr::MinusExpr(Expr* expr) : UnaryExpr(expr, EK_MinusExpr) {}

void MinusExpr::accept(ExprVisitor& visitor) {
    visitor.visit(*this);
}

bool MinusExpr::classof(const Expr* expr) {
    return expr->getKind() == EK_MinusExpr;
}

LogicalNot::LogicalNot(Expr* expr) : UnaryExpr(expr, EK_LogicalNot) {}

void LogicalNot::accept(ExprVisitor& visitor) {
    visitor.visit(*this);
}

bool LogicalNot::classof(const Expr* expr) {
    return expr->getKind() == EK_LogicalNot;
}
