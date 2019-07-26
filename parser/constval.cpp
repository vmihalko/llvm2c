#include "constval.h"

Expr* parseLLVMInstruction(const llvm::Instruction& ins, Program& program);

Expr* createUndefValue(const llvm::Type* ty, Program& program) {
    if (ty->isPointerTy()) {
        return program.addOwnership(std::make_unique<Value>("0", program.getType(ty)));
    }

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
    init->setType(program.getType(ty));
    return program.addOwnership(std::move(init));
}

Expr* createConstantValue(const llvm::Value* val, Program& program) {
    if (llvm::isa<llvm::UndefValue>(val)) {
        return createUndefValue(val->getType(), program);
    }

    if (auto CPN = llvm::dyn_cast_or_null<llvm::ConstantPointerNull>(val)) {
        return program.addOwnership(std::make_unique<Value>("0", program.getType(val->getType())));
    }

    if (const llvm::GlobalVariable* GV = llvm::dyn_cast_or_null<llvm::GlobalVariable>(val)) {
        auto RE = program.getGlobalRef(GV);
        assert(RE && "initialize-global-vars: global value does not have a ref");

        return RE;
    }


    if (llvm::isa<llvm::Function>(val)) {
        auto ref = std::make_unique<Value>(val->getName().str(), std::make_unique<PointerType>(std::make_unique<CharType>(false)));
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

        return program.addOwnership(std::make_unique<Value>(value, program.getType(CI->getType())));
    }

    if (auto CFP = llvm::dyn_cast_or_null<llvm::ConstantFP>(val)) {
        if (CFP->isInfinity()) {
            auto call = std::make_unique<CallExpr>(nullptr, "__builtin_inff", std::vector<Expr*>{}, program.getType(CFP->getType()));
            return program.addOwnership(std::move(call));
        } else if (CFP->isNaN()){
            auto param = std::make_unique<Value>("\"\"", std::make_unique<PointerType>(std::make_unique<CharType>(true)));
            auto call = std::make_unique<CallExpr>(nullptr, "__builtin_nanf", std::vector<Expr*>{param.get()}, std::make_unique<FloatType>());
            program.addOwnership(std::move(param));
            return program.addOwnership(std::move(call));
        } else {
            std::string CFPvalue = std::to_string(CFP->getValueAPF().convertToDouble());
            if (CFPvalue.compare("-nan") == 0) {
                auto param = std::make_unique<Value>("\"\"", std::make_unique<PointerType>(std::make_unique<CharType>(true)));
                auto call = std::make_unique<CallExpr>(nullptr, "-__builtin_nanf", std::vector<Expr*>{param.get()}, std::make_unique<FloatType>());
                // TODO flip sign using expression, not this hack
                program.addOwnership(std::move(param));
                return program.addOwnership(std::move(call));
            } else {
                llvm::SmallVector<char, 32> string;
                CFPvalue = "";
                CFP->getValueAPF().toString(string, 32, 0);
                for (unsigned i = 0; i < string.size(); i++) {
                    CFPvalue += string[i];
                }
            }

            return program.addOwnership(std::make_unique<Value>(CFPvalue, program.getType(CFP->getType())));
        }
    }

    if (const auto* CAZ = llvm::dyn_cast_or_null<llvm::ConstantAggregateZero>(val)) {
        std::vector<Expr*> values;

        for (int i = 0; i < CAZ->getNumElements(); ++i) {
            auto* elem = CAZ->getElementValue(i);
            values.push_back(createConstantValue(elem, program));
        }

        auto ai = std::make_unique<AggregateInitializer>(std::move(values));
        ai->setType(program.getType(val->getType()));

        return program.addOwnership(std::move(ai));
    }

    if (const llvm::ConstantAggregate* CA = llvm::dyn_cast_or_null<llvm::ConstantAggregate>(val)) {
        std::vector<Expr*> values;

        for (int i = 0; true; ++i) {
            auto* elem = CA->getAggregateElement(i);
            if (!elem)
                break;

            values.push_back(createConstantValue(elem, program));
        }

        auto ai = std::make_unique<AggregateInitializer>(std::move(values));
        ai->setType(program.getType(val->getType()));

        return program.addOwnership(std::move(ai));
    }

    if (const llvm::ConstantDataSequential* CDS = llvm::dyn_cast_or_null<llvm::ConstantDataSequential>(val)) {
        std::vector<Expr*> values;

        for (int i = 0; i < CDS->getNumElements(); ++i) {
            values.push_back(createConstantValue(CDS->getElementAsConstant(i), program));
        }

        auto ai = std::make_unique<AggregateInitializer>(values);
        ai->setType(program.getType(val->getType()));

        return program.addOwnership(std::move(ai));
    }

    if (auto CE = llvm::dyn_cast_or_null<llvm::ConstantExpr>(val)) {
        auto constExpr = const_cast<llvm::ConstantExpr*>(CE);
        if (auto* inst = constExpr->getAsInstruction()) {
            return parseLLVMInstruction(*inst, program);
        }
    }

    //if (!val->getType()->isStructTy() && !val->getType()->isPointerTy() && !val->getType()->isArrayTy()) {
    val->print(llvm::errs(), true);
    assert(false && "constval: unknown type of initial value of a global variable");
    //}

    auto ai = std::make_unique<AggregateInitializer>(std::vector<Expr*>{});
    return program.addOwnership(std::move(ai));
}
