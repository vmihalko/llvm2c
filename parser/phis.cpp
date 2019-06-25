#include "../core/Program.h"
#include "../core/Func.h"
#include "../core/Block.h"
#include "cfunc.h"
#include "constval.h"

#include <llvm/IR/Instruction.h>

static void parsePhiInstruction(const llvm::Instruction& ins, bool isConstExpr, const llvm::Value *val, Func* func, Block* block) {
    const llvm::Value* value = isConstExpr ? val : &ins;
    const auto* phi = llvm::cast<const llvm::PHINode>(&ins);
    assert(phi != nullptr && "instruction is not a phi node or is null");

    auto* phiVar = func->getExpr(value);

    // for all incoming blocks:
    for (auto i = 0; i < phi->getNumIncomingValues(); ++i) {
        auto* inBlock = phi->getIncomingBlock(i);
        auto* inValue = phi->getIncomingValue(i);

        if (!func->getExpr(inValue)) {
            createConstantValue(inValue, func, block);
        }

        // at the end of @inBlock (just before br instruction), append an assignment @value = @inValue
        auto* myBlock = func->getBlock(inBlock);
        auto assign = std::make_unique<AssignExpr>(phiVar, func->getExpr(inValue));
        myBlock->addExpr(assign.get());
        myBlock->addOwnership(std::move(assign));
    }
}

void addPhis(const llvm::Module* module, Program& program) {
    for (const auto& function : module->functions()) {
        auto* func = program.getFunction(&function);
        for (const auto& block : function) {
            auto* myBlock = func->getBlock(&block);

            for (const auto& ins : block) {
                if (ins.getOpcode() == llvm::Instruction::PHI) {
                    parsePhiInstruction(ins, false, nullptr, func, myBlock);
                }
            }
        }
    }
}
