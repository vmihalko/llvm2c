#include "../core/Program.h"
#include "../core/Func.h"
#include "../core/Block.h"
#include "../type/Type.h"

#include <llvm/IR/Instruction.h>

#include <regex>
#include <unordered_set>

using GVarSet = std::unordered_set<const llvm::GlobalValue*>;

static void parseGlobalVar(const llvm::GlobalVariable& gvar, Program& program, GVarSet& initialized);
static Expr* getInitValue(const llvm::Constant* val, Program& program, GVarSet& initialized);

static Expr* createFromConstantExpr(llvm::ConstantExpr* expr, GVarSet& initialized) {
    const llvm::Instruction* ins = expr->getAsInstruction();
    switch (ins->getOpcode()) {
    default:
        assert(false && "unknown expression used in a global variable initializer");
    }
}

static Expr* createUndefValue(const llvm::Type* ty, Program& program) {
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

static Expr* getInitValue(const llvm::Constant* val, Program& program, GVarSet& initialized) {
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
        if (!RE) {
            parseGlobalVar(*GV, program, initialized);
            RE = program.getGlobalRef(GV);
        }

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

            values.push_back(getInitValue(elem, program, initialized));
        }

        auto ai = std::make_unique<AggregateInitializer>(values);

        return program.addOwnership(std::move(ai));
    }

    if (const llvm::ConstantDataSequential* CDS = llvm::dyn_cast_or_null<llvm::ConstantDataSequential>(val)) {
        std::vector<Expr*> values;

        for (int i = 0; i < CDS->getNumElements(); ++i) {
            values.push_back(getInitValue(CDS->getElementAsConstant(i), program, initialized));
        }

        auto ai = std::make_unique<AggregateInitializer>(values);

        return program.addOwnership(std::move(ai));
    }

    if (llvm::isa<llvm::UndefValue>(val)) {
        return createUndefValue(val->getType(), program);
    }

    if (auto* CE = const_cast<llvm::ConstantExpr*>(llvm::dyn_cast_or_null<llvm::ConstantExpr>(val))) {
        return createFromConstantExpr(CE, initialized);
    }

    if (!val->getType()->isStructTy() && !val->getType()->isPointerTy() && !val->getType()->isArrayTy()) {
        val->print(llvm::errs(), true);
        assert(false && "globalVars: unknown type of initial value of a global variable");
    }

    auto ai = std::make_unique<AggregateInitializer>(std::vector<Expr*>{});
    return program.addOwnership(std::move(ai));
}

void parseGlobalVars(const llvm::Module* module, Program& program) {
    GVarSet initialized;

    for (const llvm::GlobalVariable& gvar : module->globals()) {
        if (llvm::isa<llvm::Function>(&gvar)) {
            continue;
        }

        parseGlobalVar(gvar, program, initialized);
    }
}

static void parseGlobalVar(const llvm::GlobalVariable& gvar, Program& program, GVarSet& initialized) {
    // do not initialize twice
    if (initialized.find(&gvar) != initialized.end()) {
        return;
    }

    std::string gvarName = gvar.getName().str();
    std::replace(gvarName.begin(), gvarName.end(), '.', '_');

    std::regex varName("var[0-9]+");
    if (std::regex_match(gvarName, varName)) {
        program.globalVarNames.insert(gvarName);
    }

    Expr* value = nullptr;

    llvm::PointerType* PT = llvm::cast<llvm::PointerType>(gvar.getType());

    auto var = std::make_unique<GlobalValue>(gvarName, nullptr, program.getType(PT->getElementType()));
    var->getType()->isStatic = gvar.hasInternalLinkage();

    program.globalRefs[&gvar] = std::make_unique<RefExpr>(var.get());

    if (gvar.hasInitializer()) {
        var->value = getInitValue(gvar.getInitializer(), program, initialized);
    }

    program.globalVars.push_back(std::move(var));
}
