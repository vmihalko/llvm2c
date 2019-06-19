#include "BinaryExpr.h"

#include "llvm/Support/raw_ostream.h"

#include "../type/TypeHandler.h"

/*
 * BinaryExpr classes
 */

BinaryExpr::BinaryExpr(Expr* l, Expr* r) {
    left = l;
    right = r;

    setType(TypeHandler::getBinaryType(left->getType(), right->getType()));
}

AddExpr::AddExpr(Expr* l, Expr* r) :
    BinaryExpr(l, r) { }

void AddExpr::accept(ExprVisitor& visitor) {
    visitor.visit(*this);
}

SubExpr::SubExpr(Expr* l, Expr* r) :
    BinaryExpr(l, r) { }

void SubExpr::accept(ExprVisitor& visitor) {
    visitor.visit(*this);
}

AssignExpr::AssignExpr(Expr* l, Expr* r) :
    BinaryExpr(l, r) { }

void AssignExpr::accept(ExprVisitor& visitor) {
    visitor.visit(*this);
}

MulExpr::MulExpr(Expr* l, Expr* r) :
    BinaryExpr(l, r) { }

void MulExpr::accept(ExprVisitor& visitor) {
    visitor.visit(*this);
}

DivExpr::DivExpr(Expr* l, Expr* r) :
    BinaryExpr(l, r) { }

void DivExpr::accept(ExprVisitor& visitor) {
    visitor.visit(*this);
}

RemExpr::RemExpr(Expr* l, Expr* r) :
    BinaryExpr(l, r) { }

void RemExpr::accept(ExprVisitor& visitor) {
    visitor.visit(*this);
}

AndExpr::AndExpr(Expr* l, Expr* r) :
    BinaryExpr(l, r) { }

void AndExpr::accept(ExprVisitor& visitor) {
    visitor.visit(*this);
}

OrExpr::OrExpr(Expr* l, Expr* r) :
    BinaryExpr(l, r) { }

void OrExpr::accept(ExprVisitor& visitor) {
    visitor.visit(*this);
}

XorExpr::XorExpr(Expr* l, Expr* r) :
    BinaryExpr(l, r) { }

void XorExpr::accept(ExprVisitor& visitor) {
    visitor.visit(*this);
}

CmpExpr::CmpExpr(Expr* l, Expr* r, const std::string& cmp, bool isUnsigned) :
    BinaryExpr(l,r) {
    comparsion = cmp;
    this->isUnsigned = isUnsigned;
    setType(std::make_unique<IntType>(false));
}

void CmpExpr::accept(ExprVisitor& visitor) {
    visitor.visit(*this);
}

AshrExpr::AshrExpr(Expr* l, Expr* r) :
    BinaryExpr(l, r) { }

void AshrExpr::accept(ExprVisitor& visitor) {
    visitor.visit(*this);
}

LshrExpr::LshrExpr(Expr* l, Expr* r) :
    BinaryExpr(l, r) { }

void LshrExpr::accept(ExprVisitor& visitor) {
    visitor.visit(*this);
}

ShlExpr::ShlExpr(Expr* l, Expr* r) :
    BinaryExpr(l, r) { }

void ShlExpr::accept(ExprVisitor& visitor) {
    visitor.visit(*this);
}
