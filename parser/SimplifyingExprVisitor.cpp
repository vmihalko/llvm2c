#include "SimplifyingExprVisitor.h"

void SimplifyingExprVisitor::visit(StructElement& expr) {
    expr.expr->accept(*this);
    expr.expr = simplify(expr.expr);
}

void SimplifyingExprVisitor::visit(ArrayElement& ae) {
    ae.expr->accept(*this);
    ae.element->accept(*this);

    ae.expr = simplify(ae.expr);
    ae.element = simplify(ae.element);
}

void SimplifyingExprVisitor::visit(ExtractValueExpr& expr) {
    for (auto& index : expr.indices) {
        index->accept(*this);
    }
}

void SimplifyingExprVisitor::visit(IfExpr& ifExpr) {
    ifExpr.cmp->accept(*this);
    ifExpr.cmp = simplify(ifExpr.cmp);

    ifExpr.trueList->accept(*this);
    ifExpr.trueList = simplify(ifExpr.trueList);

    ifExpr.falseList->accept(*this);
    ifExpr.falseList = simplify(ifExpr.falseList);
}

void SimplifyingExprVisitor::visit(SwitchExpr& expr) {
    expr.cmp->accept(*this);
    expr.cmp = simplify(expr.cmp);

    for (auto& pair : expr.cases) {
        pair.second->accept(*this);
        pair.second = simplify(pair.second);
    }

    expr.def->accept(*this);
    expr.def = simplify(expr.def);

}

void SimplifyingExprVisitor::visit(AsmExpr& expr) {
    for (auto& pair : expr.output) {
        if (!pair.second)
            continue;

        pair.second->accept(*this);
        pair.second = simplify(pair.second);
    }

    for (auto& pair : expr.input) {
        if (!pair.second)
            continue;

        pair.second->accept(*this);
        pair.second = simplify(pair.second);
    }
}

void SimplifyingExprVisitor::visit(CallExpr& expr) {
    if (expr.funcValue) {
        expr.funcValue->accept(*this);
        expr.funcValue = simplify(expr.funcValue);
    }

    for (auto it = expr.params.begin(); it != expr.params.end(); ++it) {
        (*it)->accept(*this);
        *it = simplify(*it);
    }
}

void SimplifyingExprVisitor::visit(PointerShift& expr) {
    expr.pointer->accept(*this);
    expr.move->accept(*this);

    expr.pointer = simplify(expr.pointer);
    expr.move = simplify(expr.move);
}

void SimplifyingExprVisitor::visit(GepExpr& expr) {
    for (auto it = expr.indices.begin(); it != expr.indices.end(); ++it) {
        (*it)->accept(*this);
        *it = simplify(*it);
    }
}

void SimplifyingExprVisitor::visit(SelectExpr& expr) {
    expr.left->accept(*this);
    expr.right->accept(*this);
    expr.comp->accept(*this);

    expr.left = simplify(expr.left);
    expr.right = simplify(expr.right);
    expr.comp = simplify(expr.comp);
}

void SimplifyingExprVisitor::visit(RefExpr& expr) {
    expr.expr->accept(*this);
    expr.expr = simplify(expr.expr);
}

void SimplifyingExprVisitor::visit(DerefExpr& expr) {
    expr.expr->accept(*this);
    expr.expr = simplify(expr.expr);
}

void SimplifyingExprVisitor::visit(RetExpr& expr) {
    if (expr.expr) {
        expr.expr->accept(*this);
        expr.expr = simplify(expr.expr);
    }
}

void SimplifyingExprVisitor::visit(CastExpr& expr) {
    expr.expr->accept(*this);
    expr.expr = simplify(expr.expr);
}

void SimplifyingExprVisitor::visit(AddExpr& expr) {
    expr.left->accept(*this);
    expr.right->accept(*this);

    expr.left = simplify(expr.left);
    expr.right = simplify(expr.right);
}

void SimplifyingExprVisitor::visit(SubExpr& expr) {
    expr.left->accept(*this);
    expr.right->accept(*this);

    expr.left = simplify(expr.left);
    expr.right = simplify(expr.right);
}

void SimplifyingExprVisitor::visit(AssignExpr& expr) {
    expr.left->accept(*this);
    expr.right->accept(*this);

    expr.left = simplify(expr.left);
    expr.right = simplify(expr.right);
}

void SimplifyingExprVisitor::visit(MulExpr& expr) {
    expr.left->accept(*this);
    expr.right->accept(*this);

    expr.left = simplify(expr.left);
    expr.right = simplify(expr.right);
}

void SimplifyingExprVisitor::visit(DivExpr& expr) {
    expr.left->accept(*this);
    expr.right->accept(*this);

    expr.left = simplify(expr.left);
    expr.right = simplify(expr.right);
}

void SimplifyingExprVisitor::visit(RemExpr& expr) {
    expr.left->accept(*this);
    expr.right->accept(*this);

    expr.left = simplify(expr.left);
    expr.right = simplify(expr.right);
}

void SimplifyingExprVisitor::visit(AndExpr& expr) {
    expr.left->accept(*this);
    expr.right->accept(*this);

    expr.left = simplify(expr.left);
    expr.right = simplify(expr.right);
}

void SimplifyingExprVisitor::visit(OrExpr& expr) {
    expr.left->accept(*this);
    expr.right->accept(*this);

    expr.left = simplify(expr.left);
    expr.right = simplify(expr.right);
}

void SimplifyingExprVisitor::visit(XorExpr& expr) {
    expr.left->accept(*this);
    expr.right->accept(*this);

    expr.left = simplify(expr.left);
    expr.right = simplify(expr.right);
}

void SimplifyingExprVisitor::visit(CmpExpr& expr) {
    expr.left->accept(*this);
    expr.right->accept(*this);

    expr.left = simplify(expr.left);
    expr.right = simplify(expr.right);
}

void SimplifyingExprVisitor::visit(AshrExpr& expr) {
    expr.left->accept(*this);
    expr.right->accept(*this);

    expr.left = simplify(expr.left);
    expr.right = simplify(expr.right);
}

void SimplifyingExprVisitor::visit(LshrExpr& expr) {
    expr.left->accept(*this);
    expr.right->accept(*this);

    expr.left = simplify(expr.left);
    expr.right = simplify(expr.right);
}

void SimplifyingExprVisitor::visit(ShlExpr& expr) {
    expr.left->accept(*this);
    expr.right->accept(*this);

    expr.left = simplify(expr.left);
    expr.right = simplify(expr.right);
}

void SimplifyingExprVisitor::visit(AggregateInitializer& expr) {
    for (auto it = expr.values.begin(); it != expr.values.end(); ++it) {
        *it = simplify(*it);
    }
}

void SimplifyingExprVisitor::visit(ExprList& expr) {
    for (auto it = expr.expressions.begin(); it != expr.expressions.end(); ++it) {
        *it = simplify(*it);
    }
}

void SimplifyingExprVisitor::visit(ArrowExpr& expr) {
    expr.expr->accept(*this);
    expr.expr = simplify(expr.expr);
}

void SimplifyingExprVisitor::visit(LogicalAnd& expr) {
    expr.lhs->accept(*this);
    expr.rhs->accept(*this);

    expr.lhs = simplify(expr.lhs);
    expr.rhs = simplify(expr.rhs);
}

void SimplifyingExprVisitor::visit(LogicalOr& expr) {
    expr.lhs->accept(*this);
    expr.rhs->accept(*this);

    expr.lhs = simplify(expr.lhs);
    expr.rhs = simplify(expr.rhs);
}

void SimplifyingExprVisitor::visit(MinusExpr& expr) {
    expr.expr->accept(*this);
    expr.expr = simplify(expr.expr);
}
