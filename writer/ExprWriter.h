#pragma once

#include <string>
#include <ostream>

#include "../expr/Expr.h"
#include "../expr/UnaryExpr.h"
#include "../expr/BinaryExpr.h"
#include "../expr/ExprVisitor.h"

class ExprWriter : public ExprVisitor
{
private:
    std::ostream& ss;
    bool noFuncCasts;
    size_t indentCount = 0;

    void gotoOrInline(Block* block);
    void parensIfNotSimple(Expr* expr);
    void indent();

public:
    ExprWriter(std::ostream& os, bool noFuncCasts);

    void visit(Struct& expr) override;
    void visit(StructElement& expr) override;
    void visit(ArrayElement& expr) override;
    void visit(ExtractValueExpr& expr) override;
    void visit(Value& expr) override;
    void visit(GlobalValue& expr) override;
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
    void visit(StackAlloc& expr) override;

    virtual ~ExprWriter() = default;
};
