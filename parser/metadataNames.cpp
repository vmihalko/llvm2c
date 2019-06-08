#include "../core/Program.h"
#include "../core/Func.h"
#include "../core/Block.h"

#include <llvm/IR/Instruction.h>

#include <regex>

void findMetadataNames(const llvm::Module* module, Program& program) {
    for (const llvm::Function& func : module->functions()) {
        auto function = program.getFunction(&func);

        function->fillMetadataVarNames(program.getGlobalVarNames());

        for (const llvm::BasicBlock& block : func) {
            for (const llvm::Instruction& ins : block) {
                if (ins.getOpcode() == llvm::Instruction::Call) {
                    const auto CI = llvm::cast<llvm::CallInst>(&ins);
                    if (CI->getCalledFunction() && CI->getCalledFunction()->getName().str().compare("llvm.dbg.declare") == 0) {
                        llvm::Metadata* varMD = llvm::dyn_cast<llvm::MetadataAsValue>(ins.getOperand(1))->getMetadata();
                        llvm::DILocalVariable* localVar = llvm::dyn_cast<llvm::DILocalVariable>(varMD);

                        std::regex varName("var[0-9]+");
                        if (std::regex_match(localVar->getName().str(), varName)) {
                            function->addMetadataVarName(localVar->getName().str());
                        }
                    }
                }
            }
        }
    }
}
