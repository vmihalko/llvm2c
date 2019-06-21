#include <llvm/IR/Instruction.h>
#include <memory>

#include "../core/Program.h"
#include "../core/Func.h"
#include "../core/Block.h"

#include "../expr/ExprVisitor.h"

class SignCastsVisitor : public ExprVisitor {
    Block* block;

    Expr* castIfNeeded(Expr* expr, bool isUnsigned);

public:
    SignCastsVisitor(Block* block) : block(block) {}

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


void addSignCasts(const llvm::Module* module, Program& program) {
    for (const llvm::Function& func : module->functions()) {
        auto* function = program.getFunction(&func);
        for (const auto& block : func) {
            auto* myBlock = function->getBlock(&block);
            SignCastsVisitor scv(myBlock);

            for (auto it = myBlock->expressions.begin(); it != myBlock->expressions.end(); ++it) {
                auto expr = *it;
                expr->accept(scv);

            }
        }
    }
}

void SignCastsVisitor::visit(IfExpr& expr) {
    if (expr.cmp) {
        expr.cmp->accept(*this);
    }
}

Expr* SignCastsVisitor::castIfNeeded(Expr* expr, bool isUnsigned) {
    Expr* result = expr;
    auto IT = llvm::dyn_cast<IntegerType>(expr->getType());

    if (IT && IT->unsignedType != isUnsigned) {
        auto newType = IT->clone();
        static_cast<IntegerType*>(newType.get())->unsignedType = isUnsigned;
        auto cast = std::make_unique<CastExpr>(expr, std::move(newType));
        result = cast.get();
        block->ownership.push_back(std::move(cast));
    }

    return result;
}

void SignCastsVisitor::visit(CmpExpr& expr) {
    expr.left = castIfNeeded(expr.left, expr.isUnsigned);
    expr.right = castIfNeeded(expr.right, expr.isUnsigned);
}

void SignCastsVisitor::visit(StructElement& expr) {
    expr.expr->accept(*this);
}

void SignCastsVisitor::visit(ArrayElement& ae) {
    ae.expr->accept(*this);
    ae.element->accept(*this);
}

void SignCastsVisitor::visit(ExtractValueExpr& expr) {
    for (auto& index : expr.indices) {
        index->accept(*this);
    }
}

void SignCastsVisitor::visit(SwitchExpr& expr) {
    expr.cmp->accept(*this);
}

void SignCastsVisitor::visit(AsmExpr& expr) {
}

void SignCastsVisitor::visit(CallExpr& expr) {
    if (expr.funcValue) {
        expr.funcValue->accept(*this);
    }

    for (auto it = expr.params.begin(); it != expr.params.end(); ++it) {
        (*it)->accept(*this);
    }
}

void SignCastsVisitor::visit(PointerShift& expr) {
    expr.pointer->accept(*this);
    expr.move->accept(*this);
}

void SignCastsVisitor::visit(GepExpr& expr) {
    for (auto& index : expr.indices) {
        index->accept(*this);
    }
}

void SignCastsVisitor::visit(SelectExpr& expr) {
    expr.left->accept(*this);
    expr.right->accept(*this);
    expr.comp->accept(*this);
}

void SignCastsVisitor::visit(RefExpr& expr) {
    expr.expr->accept(*this);
}

void SignCastsVisitor::visit(DerefExpr& expr) {
    expr.expr->accept(*this);
}

void SignCastsVisitor::visit(RetExpr& expr) {
    if (expr.expr) {
        expr.expr->accept(*this);
    }
}

void SignCastsVisitor::visit(CastExpr& expr) {
    expr.expr->accept(*this);
}

void SignCastsVisitor::visit(AddExpr& expr) {
    expr.left->accept(*this);
    expr.right->accept(*this);

    expr.left = castIfNeeded(expr.left, expr.isUnsigned);
    expr.right = castIfNeeded(expr.right, expr.isUnsigned);
}

void SignCastsVisitor::visit(SubExpr& expr) {
    expr.left->accept(*this);
    expr.right->accept(*this);

    expr.left = castIfNeeded(expr.left, expr.isUnsigned);
    expr.right = castIfNeeded(expr.right, expr.isUnsigned);
}

void SignCastsVisitor::visit(AssignExpr& expr) {
    expr.left->accept(*this);
    expr.right->accept(*this);
}

void SignCastsVisitor::visit(MulExpr& expr) {
    expr.left->accept(*this);
    expr.right->accept(*this);

    expr.left = castIfNeeded(expr.left, expr.isUnsigned);
    expr.right = castIfNeeded(expr.right, expr.isUnsigned);
}

void SignCastsVisitor::visit(DivExpr& expr) {
    expr.left->accept(*this);
    expr.right->accept(*this);

    expr.left = castIfNeeded(expr.left, expr.isUnsigned);
    expr.right = castIfNeeded(expr.right, expr.isUnsigned);
}

void SignCastsVisitor::visit(RemExpr& expr) {
    expr.left->accept(*this);
    expr.right->accept(*this);

    expr.left = castIfNeeded(expr.left, expr.isUnsigned);
    expr.right = castIfNeeded(expr.right, expr.isUnsigned);
}

void SignCastsVisitor::visit(AndExpr& expr) {
    expr.left->accept(*this);
    expr.right->accept(*this);
}

void SignCastsVisitor::visit(OrExpr& expr) {
    expr.left->accept(*this);
    expr.right->accept(*this);
}

void SignCastsVisitor::visit(XorExpr& expr) {
    expr.left->accept(*this);
    expr.right->accept(*this);
}

void SignCastsVisitor::visit(AshrExpr& expr) {
    expr.left->accept(*this);
    expr.right->accept(*this);

    expr.left = castIfNeeded(expr.left, expr.isUnsigned);
    expr.right = castIfNeeded(expr.right, expr.isUnsigned);
}

void SignCastsVisitor::visit(LshrExpr& expr) {
    expr.left->accept(*this);
    expr.right->accept(*this);

    expr.left = castIfNeeded(expr.left, expr.isUnsigned);
    expr.right = castIfNeeded(expr.right, expr.isUnsigned);
}

void SignCastsVisitor::visit(ShlExpr& expr) {
    expr.left->accept(*this);
    expr.right->accept(*this);

    expr.left = castIfNeeded(expr.left, expr.isUnsigned);
    expr.right = castIfNeeded(expr.right, expr.isUnsigned);
}
