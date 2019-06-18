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

void AddExpr::print() const {
    llvm::outs() << toString();
}

std::string AddExpr::toString() const {
    return "(" + left->toString() + ") + (" + right->toString() + ")";
}

void AddExpr::accept(ExprVisitor& visitor) {
    visitor.visit(*this);
}

SubExpr::SubExpr(Expr* l, Expr* r) :
    BinaryExpr(l, r) { }

void SubExpr::print() const {
    llvm::outs() << toString();
}

std::string SubExpr::toString() const {
    return "(" + left->toString() + ") - (" + right->toString() + ")";
}

void SubExpr::accept(ExprVisitor& visitor) {
    visitor.visit(*this);
}

AssignExpr::AssignExpr(Expr* l, Expr* r) :
    BinaryExpr(l, r) { }

void AssignExpr::print() const {
    llvm::outs() << toString();
}

std::string AssignExpr::toString() const {
    return left->toString() + " = " + right->toString() + ";";
}

void AssignExpr::accept(ExprVisitor& visitor) {
    visitor.visit(*this);
}

MulExpr::MulExpr(Expr* l, Expr* r) :
    BinaryExpr(l, r) { }

void MulExpr::print() const {
    llvm::outs() << toString();
}

std::string MulExpr::toString() const {
    return "(" + left->toString() + ") * (" + right->toString() + ")";
}

void MulExpr::accept(ExprVisitor& visitor) {
    visitor.visit(*this);
}

DivExpr::DivExpr(Expr* l, Expr* r) :
    BinaryExpr(l, r) { }

void DivExpr::print() const {
    llvm::outs() << toString();
}

std::string DivExpr::toString() const {
    return "(" + left->toString() + ") / (" + right->toString() + ")";
}

void DivExpr::accept(ExprVisitor& visitor) {
    visitor.visit(*this);
}

RemExpr::RemExpr(Expr* l, Expr* r) :
    BinaryExpr(l, r) { }

void RemExpr::print() const {
    llvm::outs() << toString();
}

std::string RemExpr::toString() const {
    return "(" + left->toString() + ") % (" + right->toString() + ")";
}

void RemExpr::accept(ExprVisitor& visitor) {
    visitor.visit(*this);
}

AndExpr::AndExpr(Expr* l, Expr* r) :
    BinaryExpr(l, r) { }

void AndExpr::print() const {
    llvm::outs() << toString();
}

std::string AndExpr::toString() const {
    return "(" + left->toString() + ") & (" + right->toString() + ")";
}

void AndExpr::accept(ExprVisitor& visitor) {
    visitor.visit(*this);
}

OrExpr::OrExpr(Expr* l, Expr* r) :
    BinaryExpr(l, r) { }

void OrExpr::print() const {
    llvm::outs() << toString();
}

std::string OrExpr::toString() const {
    return "(" + left->toString() + ") | (" + right->toString() + ")";
}

void OrExpr::accept(ExprVisitor& visitor) {
    visitor.visit(*this);
}

XorExpr::XorExpr(Expr* l, Expr* r) :
    BinaryExpr(l, r) { }

void XorExpr::print() const {
    llvm::outs() << toString();
}

std::string XorExpr::toString() const {
    return "(" + left->toString() + ") ^ (" + right->toString() + ")";
}

void XorExpr::accept(ExprVisitor& visitor) {
    visitor.visit(*this);
}

CmpExpr::CmpExpr(Expr* l, Expr* r, const std::string& cmp, bool isUnsigned) :
    BinaryExpr(l,r) {
    comparsion = cmp;
    this->isUnsigned = isUnsigned;
    setType(std::make_unique<IntType>(false));
}

void CmpExpr::print() const {
    llvm::outs() << toString();
}

std::string CmpExpr::toString() const {
    if (isUnsigned) {
        auto ITL = static_cast<IntegerType*>(left->getType());
        auto ITR = static_cast<IntegerType*>(right->getType());

        if (!ITL->unsignedType && !ITR->unsignedType) {
            return "(unsigned " + ITL->toString() + ")(" + left->toString() + ") " + comparsion + " (" + right->toString() + ")";;
        }
    }
    return "(" + left->toString() + ") " + comparsion + " (" + right->toString() + ")";
}

void CmpExpr::accept(ExprVisitor& visitor) {
    visitor.visit(*this);
}

AshrExpr::AshrExpr(Expr* l, Expr* r) :
    BinaryExpr(l, r) { }

void AshrExpr::print() const {
    llvm::outs() << toString();
}

std::string AshrExpr::toString() const {
    return "(" + left->toString() + ") >> (" + right->toString() + ")";
}

void AshrExpr::accept(ExprVisitor& visitor) {
    visitor.visit(*this);
}

LshrExpr::LshrExpr(Expr* l, Expr* r) :
    BinaryExpr(l, r) { }

void LshrExpr::print() const {
    llvm::outs() << toString();
}

std::string LshrExpr::toString() const {
    std::string ret;
    auto IT = static_cast<IntegerType*>(left->getType());
    if (!IT->unsignedType) {
        ret += "(unsigned " + IT->toString() + ")(";
    } else {
        ret += "(";
    }
    return ret + left->toString() + ") >> (" + right->toString() + ")";
}

void LshrExpr::accept(ExprVisitor& visitor) {
    visitor.visit(*this);
}

ShlExpr::ShlExpr(Expr* l, Expr* r) :
    BinaryExpr(l, r) { }

void ShlExpr::print() const {
    llvm::outs() << toString();
}

std::string ShlExpr::toString() const {
    return "(" + left->toString() + ") << (" + right->toString() + ")";
}

void ShlExpr::accept(ExprVisitor& visitor) {
    visitor.visit(*this);
}
