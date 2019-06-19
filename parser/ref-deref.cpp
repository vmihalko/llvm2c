#include <llvm/IR/Instruction.h>

#include "../core/Program.h"
#include "../core/Func.h"
#include "../core/Block.h"

#include "../expr/ExprVisitor.h"

class RefDerefVisitor : public ExprVisitor {

    Expr* simplify(Expr* expr);
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

};

void refDeref(const llvm::Module* module, Program& program) {
    RefDerefVisitor rdv;

    for (const llvm::Function& func : module->functions()) {
        auto* function = program.getFunction(&func);
        for (const auto& block : func) {
            auto* myBlock = function->getBlock(&block);

            for (auto it = myBlock->expressions.begin(); it != myBlock->expressions.end(); ++it) {
                auto expr = *it;
                expr->accept(rdv);

            }
        }
    }
}

Expr* RefDerefVisitor::simplify(Expr* expr) {
    if (auto RE = dynamic_cast<RefExpr*>(expr)) {
        if (auto DE = dynamic_cast<DerefExpr*>(RE->expr)) {
            return DE->expr;
        }
    }

    if (auto DE = dynamic_cast<DerefExpr*>(expr)) {
        if (auto RE = dynamic_cast<RefExpr*>(DE->expr)) {
            return RE->expr;
        }
    }

    return expr;
}

void RefDerefVisitor::visit(StructElement& expr) {
    expr.expr->accept(*this);
    expr.expr = simplify(expr.expr);
}

void RefDerefVisitor::visit(ArrayElement& ae) {
    ae.expr->accept(*this);
    ae.element->accept(*this);

    ae.expr = simplify(ae.expr);
    ae.element = simplify(ae.element);
}

void RefDerefVisitor::visit(ExtractValueExpr& expr) {
    for (auto& index : expr.indices) {
        index->accept(*this);
    }
}

void RefDerefVisitor::visit(IfExpr& ifExpr) {
    ifExpr.cmp->accept(*this);
    ifExpr.cmp = simplify(ifExpr.cmp);
}

void RefDerefVisitor::visit(SwitchExpr& expr) {
    expr.cmp->accept(*this);
    expr.cmp = simplify(expr.cmp);
}

void RefDerefVisitor::visit(AsmExpr& expr) {
}

void RefDerefVisitor::visit(CallExpr& expr) {
    if (expr.funcValue) {
        expr.funcValue->accept(*this);
        expr.funcValue = simplify(expr.funcValue);
    }

    for (auto it = expr.params.begin(); it != expr.params.end(); ++it) {
        (*it)->accept(*this);
        *it = simplify(*it);
    }
}

void RefDerefVisitor::visit(PointerShift& expr) {
    expr.pointer->accept(*this);
    expr.move->accept(*this);

    expr.pointer = simplify(expr.pointer);
    expr.move = simplify(expr.move);
}

void RefDerefVisitor::visit(GepExpr& expr) {
    for (auto& index : expr.indices) {
        index->accept(*this);
    }
}

void RefDerefVisitor::visit(SelectExpr& expr) {
    expr.left->accept(*this);
    expr.right->accept(*this);
    expr.comp->accept(*this);

    expr.left = simplify(expr.left);
    expr.right = simplify(expr.right);
    expr.comp = simplify(expr.comp);
}

void RefDerefVisitor::visit(RefExpr& expr) {
    expr.expr->accept(*this);
    expr.expr = simplify(expr.expr);
}

void RefDerefVisitor::visit(DerefExpr& expr) {
    expr.expr->accept(*this);
    expr.expr = simplify(expr.expr);
}

void RefDerefVisitor::visit(RetExpr& expr) {
    expr.expr->accept(*this);
    expr.expr = simplify(expr.expr);
}

void RefDerefVisitor::visit(CastExpr& expr) {
    expr.expr->accept(*this);
    expr.expr = simplify(expr.expr);
}

void RefDerefVisitor::visit(AddExpr& expr) {
    expr.left->accept(*this);
    expr.right->accept(*this);

    expr.left = simplify(expr.left);
    expr.right = simplify(expr.right);
}

void RefDerefVisitor::visit(SubExpr& expr) {
    expr.left->accept(*this);
    expr.right->accept(*this);

    expr.left = simplify(expr.left);
    expr.right = simplify(expr.right);
}

void RefDerefVisitor::visit(AssignExpr& expr) {
    expr.left->accept(*this);
    expr.right->accept(*this);

    expr.left = simplify(expr.left);
    expr.right = simplify(expr.right);
}

void RefDerefVisitor::visit(MulExpr& expr) {
    expr.left->accept(*this);
    expr.right->accept(*this);

    expr.left = simplify(expr.left);
    expr.right = simplify(expr.right);
}

void RefDerefVisitor::visit(DivExpr& expr) {
    expr.left->accept(*this);
    expr.right->accept(*this);

    expr.left = simplify(expr.left);
    expr.right = simplify(expr.right);
}

void RefDerefVisitor::visit(RemExpr& expr) {
    expr.left->accept(*this);
    expr.right->accept(*this);

    expr.left = simplify(expr.left);
    expr.right = simplify(expr.right);
}

void RefDerefVisitor::visit(AndExpr& expr) {
    expr.left->accept(*this);
    expr.right->accept(*this);

    expr.left = simplify(expr.left);
    expr.right = simplify(expr.right);
}

void RefDerefVisitor::visit(OrExpr& expr) {
    expr.left->accept(*this);
    expr.right->accept(*this);

    expr.left = simplify(expr.left);
    expr.right = simplify(expr.right);
}

void RefDerefVisitor::visit(XorExpr& expr) {
    expr.left->accept(*this);
    expr.right->accept(*this);

    expr.left = simplify(expr.left);
    expr.right = simplify(expr.right);
}

void RefDerefVisitor::visit(CmpExpr& expr) {
    expr.left->accept(*this);
    expr.right->accept(*this);

    expr.left = simplify(expr.left);
    expr.right = simplify(expr.right);
}

void RefDerefVisitor::visit(AshrExpr& expr) {
    expr.left->accept(*this);
    expr.right->accept(*this);

    expr.left = simplify(expr.left);
    expr.right = simplify(expr.right);
}

void RefDerefVisitor::visit(LshrExpr& expr) {
    expr.left->accept(*this);
    expr.right->accept(*this);

    expr.left = simplify(expr.left);
    expr.right = simplify(expr.right);
}

void RefDerefVisitor::visit(ShlExpr& expr) {
    expr.left->accept(*this);
    expr.right->accept(*this);

    expr.left = simplify(expr.left);
    expr.right = simplify(expr.right);
}
