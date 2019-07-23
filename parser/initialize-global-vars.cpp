#include "../core/Program.h"
#include "../core/Func.h"
#include "../core/Block.h"
#include "../type/Type.h"

#include "constval.h"

#include <llvm/IR/Instruction.h>

#include <regex>
#include <unordered_set>

Expr* parseLLVMInstruction(const llvm::Instruction& ins, Program& program);
static Expr* getInitValue(const llvm::Constant* val, Program& program);

static Expr* getInitValue(const llvm::Constant* val, Program& program) {
    std::string name = val->getName().str();

    if (llvm::isa<llvm::Function>(val)) {
        auto f = std::make_unique<Value>(name, std::make_unique<PointerType>(std::make_unique<CharType>(false)));
        auto ref = std::make_unique<RefExpr>(f.get());
        program.addOwnership(std::move(f));
        return program.addOwnership(std::move(ref));
    }

    if (llvm::isa<llvm::ConstantPointerNull>(val)) {
        auto zero = std::make_unique<Value>("0", program.getType(val->getType()));
        return program.addOwnership(std::move(zero));
    }

    if (const llvm::GlobalVariable* GV = llvm::dyn_cast_or_null<llvm::GlobalVariable>(val)) {
        auto RE = program.getGlobalRef(GV);
        assert(RE && "initialize-global-vars: global value does not have a ref");

        return RE;
    }


    if (const llvm::ConstantInt* CI = llvm::dyn_cast_or_null<llvm::ConstantInt>(val)) {
        std::string value;
        if (CI->getBitWidth() > 64) {
            const llvm::APInt& API = CI->getValue();
            value = std::to_string(API.getLimitedValue());
        } else if (CI->getBitWidth() == 1) { //bool in LLVM
            value = std::to_string(-1 * CI->getSExtValue());
        } else {
            value = std::to_string(CI->getSExtValue());
        }

        auto num = std::make_unique<Value>(value, program.getType(CI->getType()));
        return program.addOwnership(std::move(num));
    }

    if (const llvm::ConstantFP* CFP = llvm::dyn_cast_or_null<llvm::ConstantFP>(val)) {
        if (CFP->isInfinity()) {
            auto call = std::make_unique<CallExpr>(nullptr, "__builtin_inff", std::vector<Expr*>{}, program.getType(CFP->getType()));
            return program.addOwnership(std::move(call));
        }

        if (CFP->isNaN()) {
            auto param = std::make_unique<Value>("\"\"", std::make_unique<PointerType>(std::make_unique<CharType>(true)));
            auto call = std::make_unique<CallExpr>(nullptr, "__builtin_nanf", std::vector<Expr*>{param.get()}, std::make_unique<FloatType>());
            program.addOwnership(std::move(param));
            return program.addOwnership(std::move(call));
        }

        std::string ret = std::to_string(CFP->getValueAPF().convertToDouble());
        if (ret.compare("-nan") == 0) {
            auto param = std::make_unique<Value>("\"\"", std::make_unique<PointerType>(std::make_unique<CharType>(true)));
            auto call = std::make_unique<CallExpr>(nullptr, "__builtin_nanf", std::vector<Expr*>{param.get()}, std::make_unique<FloatType>());
            // TODO flip sign like this: -(__builtin_nanf(""))
            program.addOwnership(std::move(param));
            return program.addOwnership(std::move(call));
        }

        llvm::SmallVector<char, 32> string;
        ret = "";
        CFP->getValueAPF().toString(string, 32, 0);
        for (unsigned i = 0; i < string.size(); i++) {
            ret += string[i];
        }

        auto val = std::make_unique<Value>(ret, program.getType(CFP->getType()));
        return program.addOwnership(std::move(val));
    }

    if (const llvm::ConstantAggregate* CA = llvm::dyn_cast_or_null<llvm::ConstantAggregate>(val)) {
        std::vector<Expr*> values;

        for (int i = 0; true; ++i) {
            auto* elem = CA->getAggregateElement(i);
            if (!elem)
                break;

            values.push_back(getInitValue(elem, program));
        }

        auto ai = std::make_unique<AggregateInitializer>(values);

        return program.addOwnership(std::move(ai));
    }

    if (const llvm::ConstantDataSequential* CDS = llvm::dyn_cast_or_null<llvm::ConstantDataSequential>(val)) {
        std::vector<Expr*> values;

        for (int i = 0; i < CDS->getNumElements(); ++i) {
            values.push_back(getInitValue(CDS->getElementAsConstant(i), program));
        }

        auto ai = std::make_unique<AggregateInitializer>(values);

        return program.addOwnership(std::move(ai));
    }

    if (llvm::isa<llvm::UndefValue>(val)) {
        return createUndefValue(val->getType(), program);
    }

    if (auto* CE = const_cast<llvm::ConstantExpr*>(llvm::dyn_cast_or_null<llvm::ConstantExpr>(val))) {
        if (auto* inst = CE->getAsInstruction()) {
            return parseLLVMInstruction(*inst, program);
        }
    }

    if (!val->getType()->isStructTy() && !val->getType()->isPointerTy() && !val->getType()->isArrayTy()) {
        val->print(llvm::errs(), true);
        assert(false && "globalVars: unknown type of initial value of a global variable");
    }

    auto ai = std::make_unique<AggregateInitializer>(std::vector<Expr*>{});
    return program.addOwnership(std::move(ai));
}

void initializeGlobalVars(const llvm::Module* module, Program& program) {
    for (const llvm::GlobalVariable& gvar : module->globals()) {
        if (llvm::isa<llvm::Function>(&gvar)) {
            continue;
        }

        if (gvar.hasInitializer()) {
            auto* ref = program.getGlobalVar(&gvar);
            auto* var = llvm::cast<GlobalValue>(ref->expr);
            var->value = getInitValue(gvar.getInitializer(), program);
        }
    }
}
