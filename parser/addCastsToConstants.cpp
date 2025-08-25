#include <llvm/IR/Instruction.h>

#include "../core/Program.h"
#include "../core/Func.h"
#include "../core/Block.h"

#include "SimplifyingExprVisitor.h"

class AddCastsToConstantsVisitor: public SimplifyingExprVisitor {
public:
    Block *crrntBlk;
protected:
    Expr* simplify(Expr* expr) override;
    void visit(AssignExpr& expr) override;
};

void addCastsToConstants(const llvm::Module* module, Program& program) {
    assert(program.isPassCompleted(PassType::CreateExpressions));

    AddCastsToConstantsVisitor rcv;

    for (const llvm::Function& func : module->functions()) {
        auto* function = program.getFunction(&func);
        for (const auto& block : func) {
            auto* myBlock = function->getBlock(&block);
            rcv.crrntBlk = myBlock;

            for (auto it = myBlock->expressions.begin(); it != myBlock->expressions.end(); ++it) {
                auto expr = *it;
                expr->accept(rcv);

            }
        }
    }
    program.addPass(PassType::addCastsToConstants);
}

void AddCastsToConstantsVisitor::visit(AssignExpr&) {
    return;
}
#include <string.h>

bool is_number(const std::string& s)
{
    return( strspn( s.c_str(), "-.0123456789" ) == s.size() );
}
//do THIS only if we are not in assignment
Expr* AddCastsToConstantsVisitor::simplify(Expr* expr) {
    Expr * result = expr;
    // if (auto* isValue = llvm::dyn_cast_or_null<Value>(expr)) {
    //     if (is_number(isValue->valueName) && isValue->valueName[0] == '-' ) /* negative number */ {
    //         if(auto IT = llvm::dyn_cast_or_null<IntegerType>(isValue->getType())) {
    //             if (IT)
    //         }
    //         result a= cast.get();
    //         crrntBlk->addOwnership(std::move(cast));
    //     }
    // }
    return result;
}
