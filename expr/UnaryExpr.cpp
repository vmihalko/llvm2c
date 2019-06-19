#include "UnaryExpr.h"

#include "llvm/Support/raw_ostream.h"

/*
 * UnaryExpr classes
 */

UnaryExpr::UnaryExpr(Expr *expr) {
    this->expr = expr;
    if (expr) {
        setType(expr->getType()->clone());
    }
}

RefExpr::RefExpr(Expr* expr) :
    UnaryExpr(expr) {
    setType(std::make_unique<PointerType>(expr->getType()->clone()));
}

void RefExpr::accept(ExprVisitor& visitor) {
    visitor.visit(*this);
}

DerefExpr::DerefExpr(Expr* expr) :
    UnaryExpr(expr) {
    if (auto PT = dynamic_cast<PointerType*>(expr->getType())) {
        setType(PT->type->clone());
    }
}

void DerefExpr::accept(ExprVisitor& visitor) {
    visitor.visit(*this);
}

RetExpr::RetExpr(Expr* ret)
    : UnaryExpr(ret) { }

RetExpr::RetExpr()
    : UnaryExpr(nullptr) { }

void RetExpr::accept(ExprVisitor& visitor) {
    visitor.visit(*this);
}

CastExpr::CastExpr(Expr* expr, std::unique_ptr<Type> type)
    : UnaryExpr(expr) {
    setType(std::move(type));
}

void CastExpr::accept(ExprVisitor& visitor) {
    visitor.visit(*this);
}

