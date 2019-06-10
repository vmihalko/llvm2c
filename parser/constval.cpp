#include "constval.h"

void parseLLVMInstruction(const llvm::Instruction& ins, bool isConstExpr, const llvm::Value* val, Func* func, Block *block);

void createConstantValue(const llvm::Value* val, Func* func, Block* block) {
    //undefined value is translated as zero, only for experimental purposes (this value cannot occur in LLVM generated from C)
    if (llvm::isa<llvm::UndefValue>(val)) {
        func->createExpr(val, std::make_unique<Value>("0", func->getType(val->getType())));
        return;
    }

    if (auto CPN = llvm::dyn_cast<llvm::ConstantPointerNull>(val)) {
        func->createExpr(val, std::make_unique<Value>("0", func->getType(CPN->getType())));
        return;
    }

    if (auto CI = llvm::dyn_cast<llvm::ConstantInt>(val)) {
        std::string value;
        if (CI->getBitWidth() > 64) {
            const llvm::APInt& API = CI->getValue();
            value = std::to_string(API.getLimitedValue());
        } else if (CI->getBitWidth() == 1) { //bool in LLVM
            value = std::to_string(-1 * CI->getSExtValue());
        } else {
            value = std::to_string(CI->getSExtValue());
        }

        func->createExpr(val, std::make_unique<Value>(value, std::make_unique<IntType>(false)));
        return;
    }

    if (auto CFP = llvm::dyn_cast<llvm::ConstantFP>(val)) {
        if (CFP->isInfinity()) {
            func->createExpr(val, std::make_unique<Value>("__builtin_inff ()", std::make_unique<FloatType>()));
        } else if (CFP->isNaN()){
            func->createExpr(val, std::make_unique<Value>("__builtin_nanf (\"\")", std::make_unique<FloatType>()));
        } else {
            std::string CFPvalue = std::to_string(CFP->getValueAPF().convertToDouble());
            if (CFPvalue.compare("-nan") == 0) {
                CFPvalue = "-(__builtin_nanf (\"\"))";
            } else {
                llvm::SmallVector<char, 32> string;
                CFPvalue = "";
                CFP->getValueAPF().toString(string, 32, 0);
                for (unsigned i = 0; i < string.size(); i++) {
                    CFPvalue += string[i];
                }
            }

            func->createExpr(val, std::make_unique<Value>(CFPvalue, std::make_unique<FloatType>()));
        }
        return;
    }

    if (auto CE = llvm::dyn_cast<llvm::ConstantExpr>(val)) {
        parseLLVMInstruction(*CE->getAsInstruction(), true, val, func, block);
    }
}

