#pragma once

#include "../expr/Expr.h"
#include "../expr/UnaryExpr.h"
#include "../expr/BinaryExpr.h"
#include "../expr/ExprVisitor.h"

class SimplifyingExprVisitor : public ExprVisitor
{

protected:
    virtual Expr* simplify(Expr* expr) = 0;
public:
    void visit(StructElement& expr) override;
    void visit(ArrayElement& expr) override;
    void visit(ExtractValueExpr& expr) override;
    void visit(IfExpr& expr) override;
    void visit(SwitchExpr& expr) override;
    void visit(AsmExpr& expr) override;
    void visit(CallExpr& expr) override;
    void visit(PointerShift& expr) override;
    void visit(GepExpr& expr) override;
    void visit(SelectExpr& expr) override;
    void visit(RefExpr& expr) override;
    void visit(DerefExpr& expr) override;
    void visit(RetExpr& expr) override;
    void visit(CastExpr& expr) override;
    void visit(AddExpr& expr) override;
    void visit(SubExpr& expr) override;
    void visit(AssignExpr& expr) override;
    void visit(MulExpr& expr) override;
    void visit(DivExpr& expr) override;
    void visit(RemExpr& expr) override;
    void visit(AndExpr& expr) override;
    void visit(OrExpr& expr) override;
    void visit(XorExpr& expr) override;
    void visit(CmpExpr& expr) override;
    void visit(AshrExpr& expr) override;
    void visit(LshrExpr& expr) override;
    void visit(ShlExpr& expr) override;
    void visit(AggregateInitializer& expr) override;
    void visit(ArrowExpr& expr) override;

    virtual ~SimplifyingExprVisitor() = default;
};
