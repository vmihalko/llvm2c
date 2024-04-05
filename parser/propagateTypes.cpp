#include <llvm/IR/Instruction.h>

#include "../core/Program.h"
#include "../core/Func.h"
#include "../core/Block.h"

#include "SimplifyingExprVisitor.h"

class PropagateTypesVisitor: public SimplifyingExprVisitor {
public:
	const llvm::Module* mdl;
	Program *prgrm;
    PropagateTypesVisitor(const llvm::Module* mdl, Program* prgrm) : mdl(mdl), prgrm(prgrm) {}
protected:
    void visit(AssignExpr& expr) override;
    void visit(SelectExpr& expr) override;
    void visit(CallExpr& expr) override;
    //void visit(Value& expr) override;
    Expr* simplify(Expr* expr) override;
};

void propagateTypes(const llvm::Module* module, Program& program) {
    assert(program.isPassCompleted(PassType::CreateExpressions));

    PropagateTypesVisitor rcv(module, &program);

    for (const llvm::Function& func : module->functions()) {
        auto* function = program.getFunction(&func);
        for (const auto& block : func) {
            auto* myBlock = function->getBlock(&block);

            for (auto it = myBlock->expressions.begin(); it != myBlock->expressions.end(); ++it) {
                auto expr = *it;
                expr->accept(rcv);

            }
        }
    }
    program.addPass(PassType::PropagateTypes);
}
void PropagateTypesVisitor::visit(CallExpr& expr) {
    if (expr.funcValue) {
        expr.funcValue->accept(*this);
        expr.funcValue = simplify(expr.funcValue);
    }

    for (auto it = expr.params.begin(); it != expr.params.end(); ++it) {
        (*it)->accept(*this);
        *it = simplify(*it);
    }
    	
    
}

void PropagateTypesVisitor::visit(AssignExpr& expr) {
    expr.left->accept(*this);
    expr.right->accept(*this);

    expr.left = simplify(expr.left);
    expr.right = simplify(expr.right);
    //llvm::errs() << "Set type to; " << expr.right->getType()->toString() << "\n";
    //expr.left->setType(expr.right->getType());
}

void PropagateTypesVisitor::visit(SelectExpr& expr) {
    expr.left->accept(*this);
    expr.right->accept(*this);
    expr.comp->accept(*this);

    expr.setType(expr.left->getType());
}
//void PropagateTypesVisitor::visit(AssignExpr& expr) {

Expr* PropagateTypesVisitor::simplify(Expr* expr) { return expr;}
