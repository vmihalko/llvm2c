#pragma once

class AggregateElement;
class ArrayElement;
class ExtractValueExpr;
class Value;
class GlobalValue;
class IfExpr;
class SwitchExpr;
class AsmExpr;
class CallExpr;
class PointerShift;
class GepExpr;
class SelectExpr;
class RefExpr;
class DerefExpr;
class RetExpr;
class CastExpr;
class AddExpr;
class SubExpr;
class AssignExpr;
class MulExpr;
class DivExpr;
class RemExpr;
class AndExpr;
class OrExpr;
class XorExpr;
class CmpExpr;
class AshrExpr;
class LshrExpr;
class ShlExpr;
class StackAlloc;
class AggregateInitializer;
class ArrowExpr;
class LogicalAnd;
class LogicalOr;
class LogicalNot;
class GotoExpr;
class ExprList;
class MinusExpr;
class DoWhile;

class ExprVisitor {
public:
    virtual void visit(AggregateElement& ) {}
    virtual void visit(ArrayElement& ) {}
    virtual void visit(ExtractValueExpr& ) {}
    virtual void visit(Value& ) {}
    virtual void visit(GlobalValue& ) {}
    virtual void visit(IfExpr& ) {}
    virtual void visit(SwitchExpr& ) {}
    virtual void visit(AsmExpr& ) {}
    virtual void visit(CallExpr& ) {}
    virtual void visit(PointerShift& ) {}
    virtual void visit(GepExpr& ) {}
    virtual void visit(SelectExpr& ) {}
    virtual void visit(RefExpr& ) {}
    virtual void visit(DerefExpr& ) {}
    virtual void visit(RetExpr& ) {}
    virtual void visit(CastExpr& ) {}
    virtual void visit(AddExpr& ) {}
    virtual void visit(SubExpr& ) {}
    virtual void visit(AssignExpr& ) {}
    virtual void visit(MulExpr& ) {}
    virtual void visit(DivExpr& ) {}
    virtual void visit(RemExpr& ) {}
    virtual void visit(AndExpr& ) {}
    virtual void visit(OrExpr& ) {}
    virtual void visit(XorExpr& ) {}
    virtual void visit(CmpExpr& ) {}
    virtual void visit(AshrExpr& ) {}
    virtual void visit(LshrExpr& ) {}
    virtual void visit(ShlExpr& ) {}
    virtual void visit(StackAlloc& ) {}
    virtual void visit(AggregateInitializer& ) {}
    virtual void visit(ArrowExpr& ) {}
    virtual void visit(LogicalAnd& ) {}
    virtual void visit(LogicalOr& ) {}
    virtual void visit(GotoExpr& ) {}
    virtual void visit(ExprList& ) {}
    virtual void visit(MinusExpr& ) {}
    virtual void visit(LogicalNot& ) {}
    virtual void visit(DoWhile& ) {}

    virtual ~ExprVisitor() = default;
};
