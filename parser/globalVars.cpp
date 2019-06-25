#include "../core/Program.h"
#include "../core/Func.h"
#include "../core/Block.h"

#include <llvm/IR/Instruction.h>

#include <regex>

static void parseGlobalVar(const llvm::GlobalVariable& gvar, Program& program);

static std::string getInitValue(const llvm::Constant* val, Program& program) {
    if (llvm::PointerType* PT = llvm::dyn_cast_or_null<llvm::PointerType>(val->getType())) {
        std::string name = val->getName().str();

        if (llvm::isa<llvm::GlobalVariable>(val)) {
            std::replace(name.begin(), name.end(), '.', '_');
        }

        if (name.empty()) {
            return "0";
        }

        if (llvm::isa<llvm::ConstantPointerNull>(val)) {
            return "0";
        }

        if (llvm::isa<llvm::FunctionType>(PT->getElementType())) {
            return  "&" + name;
        }

        if (llvm::isa<llvm::StructType>(PT->getElementType())) {
            return "&" + name;
        }

        if (const llvm::GlobalVariable* GV = llvm::dyn_cast_or_null<llvm::GlobalVariable>(val->getOperand(0))) {
            auto RE = program.getGlobalRef(GV);
            if (!RE) {
                parseGlobalVar(*GV, program);
                RE = program.getGlobalRef(GV);
            }
            auto GVAL = static_cast<GlobalValue*>(RE->expr);
            if (!GVAL->isDefined) {
                parseGlobalVar(*GV, program);
            }

            GVAL->isDefined = true;
            return GVAL->valueName;
        }

        return "&" + name;
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

        return value;
    }

    if (const llvm::ConstantFP* CFP = llvm::dyn_cast_or_null<llvm::ConstantFP>(val)) {
        if (CFP->isInfinity()) {
            return "__builtin_inff ()";
        }

        if (CFP->isNaN()) {
            return "__builtin_nanf (\"\")";
        }

        std::string ret = std::to_string(CFP->getValueAPF().convertToDouble());
        if (ret.compare("-nan") == 0) {
            return "-(__builtin_nanf (\"\"))";
        }

        llvm::SmallVector<char, 32> string;
        ret = "";
        CFP->getValueAPF().toString(string, 32, 0);
        for (unsigned i = 0; i < string.size(); i++) {
            ret += string[i];
        }

        return ret;
    }

    if (const llvm::ConstantDataArray* CDA = llvm::dyn_cast_or_null<llvm::ConstantDataArray>(val)) {
        std::string value = "{";
        bool first = true;

        for (unsigned i = 0; i < CDA->getNumElements(); i++) {
            if (!first) {
                value += ", ";
            }
            first = false;

            value += getInitValue(CDA->getElementAsConstant(i), program);
        }

        return value + "}";
    }

    if (const llvm::ConstantStruct* CS = llvm::dyn_cast_or_null<llvm::ConstantStruct>(val)) {
        std::string value = "{";
        bool first = true;
        for (unsigned i = 0; i < CS->getNumOperands(); i++) {
            if (!first) {
                value += ", ";
            }
            first = false;

            value += getInitValue(llvm::cast<llvm::Constant>(val->getOperand(i)), program);
        }

        return value + "}";
    }

    if (!val->getType()->isStructTy() && !val->getType()->isPointerTy() && !val->getType()->isArrayTy()) {
        return "0";
    }

    return "{}";
}
void parseGlobalVars(const llvm::Module* module, Program& program) {
    for (const llvm::GlobalVariable& gvar : module->globals()) {
        if (llvm::isa<llvm::Function>(&gvar)) {
            continue;
        }

        parseGlobalVar(gvar, program);
    }
}

static void parseGlobalVar(const llvm::GlobalVariable& gvar, Program& program) {
    std::string gvarName = gvar.getName().str();
    std::replace(gvarName.begin(), gvarName.end(), '.', '_');

    std::regex varName("var[0-9]+");
    if (std::regex_match(gvarName, varName)) {
        program.globalVarNames.insert(gvarName);
    }

    std::string value;
    if (gvar.hasInitializer()) {
        value = getInitValue(gvar.getInitializer(), program);
    }

    llvm::PointerType* PT = llvm::cast<llvm::PointerType>(gvar.getType());

    auto var = std::make_unique<GlobalValue>(gvarName, value, program.getType(PT->getElementType()));
    var->getType()->isStatic = gvar.hasInternalLinkage();

    program.globalRefs[&gvar] = std::make_unique<RefExpr>(var.get());
    program.globalVars.push_back(std::move(var));
}
