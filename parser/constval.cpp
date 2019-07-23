#include "constval.h"

Expr* parseLLVMInstruction(const llvm::Instruction& ins, Program& program);

Expr* createUndefValue(const llvm::Type* ty, Program& program) {
    if (ty->isIntegerTy()) {
        auto zero = std::make_unique<Value>("0", program.getType(ty));
        return program.addOwnership(std::move(zero));
    }

    std::vector<Expr*> values;

    if (auto* ST = llvm::dyn_cast_or_null<llvm::SequentialType>(ty)) {
        for (auto i = 0; i < ST->getNumElements(); ++i) {
            values.push_back(createUndefValue(ST->getElementType(), program));
        }

    } else if (auto* ST = llvm::dyn_cast_or_null<llvm::StructType>(ty)) {
        for (auto i = 0; i < ST->getNumElements(); ++i) {
            values.push_back(createUndefValue(ST->getElementType(i), program));
        }
    } else {
        ty->print(llvm::errs(), true);
        assert(false && "globalVars: unrecognized type of undef value");
    }

    auto init = std::make_unique<AggregateInitializer>(values);
    return program.addOwnership(std::move(init));
}

Expr* createConstantValue(const llvm::Value* val, Program& program) {
    //undefined value is translated as zero, only for experimental purposes (this value cannot occur in LLVM generated from C)
    if (llvm::isa<llvm::UndefValue>(val)) {
        return createUndefValue(val->getType(), program);
    }

    if (auto CPN = llvm::dyn_cast_or_null<llvm::ConstantPointerNull>(val)) {
        return program.addOwnership(std::make_unique<Value>("0", program.getType(val->getType())));
    }

    if (llvm::isa<llvm::Function>(val)) {
        auto f = std::make_unique<Value>(val->getName().str(), std::make_unique<PointerType>(std::make_unique<CharType>(false)));
        auto ref = std::make_unique<RefExpr>(f.get());
        program.addOwnership(std::move(f));
        return program.addOwnership(std::move(ref));
    }

    if (auto CI = llvm::dyn_cast_or_null<llvm::ConstantInt>(val)) {
        std::string value;
        if (CI->getBitWidth() > 64) {
            const llvm::APInt& API = CI->getValue();
            value = std::to_string(API.getLimitedValue());
        } else if (CI->getBitWidth() == 1) { //bool in LLVM
            value = std::to_string(-1 * CI->getSExtValue());
        } else {
            value = std::to_string(CI->getSExtValue());
        }

        return program.addOwnership(std::make_unique<Value>(value, std::make_unique<IntType>(false)));
    }

    if (auto CFP = llvm::dyn_cast_or_null<llvm::ConstantFP>(val)) {
        std::unique_ptr<Expr> result;
        if (CFP->isInfinity()) {
            result = std::make_unique<Value>("__builtin_inff ()", std::make_unique<FloatType>());
        } else if (CFP->isNaN()){
            result = std::make_unique<Value>("__builtin_nanf (\"\")", std::make_unique<FloatType>());
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

            result = std::make_unique<Value>(CFPvalue, std::make_unique<FloatType>());
        }
        return program.addOwnership(std::move(result));
    }

    if (auto CE = llvm::dyn_cast_or_null<llvm::ConstantExpr>(val)) {
        auto constExpr = const_cast<llvm::ConstantExpr*>(CE);
        if (auto* inst = constExpr->getAsInstruction()) {
            return parseLLVMInstruction(*inst, program);
        }
    }

    val->print(llvm::errs());
    assert(false && "Failed to create a constant value");
}
