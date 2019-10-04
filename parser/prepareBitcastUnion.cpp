#include "../type/Type.h"
#include "../core/Program.h"

#include <llvm/IR/Instruction.h>
#include <unordered_set>
#include <vector>

void prepareBitcastUnion(const llvm::Module* mod, Program& program) {
    assert(!program.isPassCompleted(PassType::CreateExpressions));

    std::unordered_set<Type*> types;

    for (const auto& function : mod->functions()) {
        for (const auto& bb : function) {
            for (const auto& ins : bb) {
                if (ins.getOpcode() == llvm::Instruction::BitCast) {
                    const auto* cast = static_cast<const llvm::BitCastInst *>(&ins);
                    types.insert(program.getType(cast->getSrcTy()));
                    types.insert(program.getType(cast->getDestTy()));
                }
            }
        }
    }

    if (!types.empty()) {
        std::vector<Type*> typeVector(types.begin(), types.end());

        std::sort(typeVector.begin(), typeVector.end(), [](const Type* a, const Type* b) {
                  return a->toString().compare(b->toString());
          });

        program.bitcastUnion = program.addUnion(typeVector);
    }

    program.addPass(PassType::PrepareBitcastUnion);
}
