#include <llvm/IR/Instruction.h>
#include <iostream>

#include "../core/Program.h"
#include "../core/Func.h"
#include "../core/Block.h"

#include "SimplifyingExprVisitor.h"

static void deleteExprFromBlock(Block* block, Expr* toDelete) {
    for (auto it = block->expressions.begin(); it != block->expressions.end(); ++it) {
        if (auto refExpr = llvm::dyn_cast_or_null<RefExpr>(toDelete)) {
            auto valueFromRefExpr = refExpr->expr;
            if (auto stackAlloca = llvm::dyn_cast_or_null<StackAlloc>(*it)) {
                auto valueFromStackAlloca = stackAlloca->value;
                if (valueFromStackAlloca == valueFromRefExpr) {
                    block->expressions.erase(it);
                    return;
                }
            } 
        } else if (auto assignExpr = llvm::dyn_cast_or_null<AssignExpr>(*it)) {
                if (assignExpr->right == toDelete) {
                    block->expressions.erase(it);
                    return;
                }
        }
    }
}

void deleteUnusedVariables(const llvm::Module* module, Program& program) {
    assert(program.isPassCompleted(PassType::CreateExpressions));
    for (const llvm::Function& func : module->functions()) {
        auto* function = program.getFunction(&func);
        for (const auto& block : func) {
            auto* myBlock = function->getBlock(&block);

            for (const auto& ins : block) {
                if (ins.getOpcode() == llvm::Instruction::Alloca) {
                    if (ins.hasOneUse()) {
                        auto* alloca = function->getExpr(&ins);
                        auto* user = llvm::dyn_cast_or_null<llvm::StoreInst>(*ins.users().begin());
                        if (!user) {
                            continue;
                        }

                        // make sure the allocation is only used as a target of store
                        if (user->getValueOperand() != &ins && user->getPointerOperand() == &ins) {
                            deleteExprFromBlock(myBlock, alloca);
                            deleteExprFromBlock(myBlock, function->getExpr(user->getOperand(0)));
                        }
                    }
                }
            }
        }
    }

    program.addPass(PassType::DeleteUnusedVariables);
}
