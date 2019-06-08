#include "../core/Program.h"
#include "../core/Func.h"
#include "../core/Block.h"
#include "cfunc.h"

#include <llvm/IR/Instruction.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/GetElementPtrTypeIterator.h>

using CaseHandle = const llvm::SwitchInst::CaseHandleImpl<const llvm::SwitchInst, const llvm::ConstantInt, const llvm::BasicBlock>*;

static void parseLLVMInstruction(const llvm::Instruction& ins, bool isConstExpr, const llvm::Value* val, Func* func, Block *block);
static void parseInlineASM(const llvm::Instruction& ins, Func* func, Block* block);

static void createConstantValue(const llvm::Value* val, Func* func, Block* block) {
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

static void createFuncCallParam(const llvm::Use& param, Func* func, Block* block) {
    if (llvm::PointerType* PT = llvm::dyn_cast<llvm::PointerType>(param->getType())) {
        if (llvm::isa<llvm::ConstantPointerNull>(param)) {
            createConstantValue(param, func, block);
        } else if (PT->getElementType()->isFunctionTy() && !param->getName().empty()) {
            func->createExpr(param, std::make_unique<Value>(param->getName().str(), std::make_unique<VoidType>()));
        } else {
            createConstantValue(param, func, block);
        }
    } else {
        createConstantValue(param, func, block);
    }
}

static void parseExtractValueInstruction(const llvm::Instruction& ins, bool isConstExpr, const llvm::Value* val, Func* func, Block* block) {
    const llvm::ExtractValueInst* EVI = llvm::cast<const llvm::ExtractValueInst>(&ins);

    std::vector<std::unique_ptr<Expr>> indices;
    std::unique_ptr<Type> prevType = func->getType(ins.getOperand(0)->getType());
    Expr* expr = func->getExpr(ins.getOperand(0));

    if (dynamic_cast<AsmExpr*>(expr)) {
        return;
    }

    for (unsigned idx : EVI->getIndices()) {
        std::unique_ptr<Expr> element = nullptr;

        if (StructType* ST = dynamic_cast<StructType*>(prevType.get())) {
            element = std::make_unique<StructElement>(func->getStruct(ST->name), expr, idx);
        }

        if (dynamic_cast<ArrayType*>(prevType.get())) {
            auto newVal = std::make_unique<Value>(std::to_string(idx), std::make_unique<IntType>(true));
            element = std::make_unique<ArrayElement>(expr, newVal.get());
            block->addValue(std::move(newVal));
        }

        prevType = element->getType()->clone();
        expr = element.get();
        indices.push_back(std::move(element));
    }

    func->createExpr(isConstExpr ? val : &ins, std::make_unique<ExtractValueExpr>(indices));
}

static void parseCmpInstruction(const llvm::Instruction& ins, bool isConstExpr, const llvm::Value* val, Func* func, Block* block) {
    if (func->getExpr(ins.getOperand(0)) == nullptr) {
        createConstantValue(ins.getOperand(0), func, block);
    }
    Expr* val0 = func->getExpr(ins.getOperand(0));

    if (func->getExpr(ins.getOperand(1)) == nullptr) {
        createConstantValue(ins.getOperand(1), func, block);
    }
    Expr* val1 = func->getExpr(ins.getOperand(1));

    auto cmpInst = llvm::cast<const llvm::CmpInst>(&ins);
    const llvm::Value* value = isConstExpr ? val : &ins;

    switch(cmpInst->getPredicate()) {
    case llvm::CmpInst::ICMP_EQ:
    case llvm::CmpInst::FCMP_OEQ:
    case llvm::CmpInst::FCMP_UEQ:
        func->createExpr(value, std::make_unique<CmpExpr>(val0, val1, "==", false));
        break;
    case llvm::CmpInst::ICMP_NE:
    case llvm::CmpInst::FCMP_ONE:
    case llvm::CmpInst::FCMP_UNE:
        func->createExpr(value, std::make_unique<CmpExpr>(val0, val1, "!=", false));
        break;
    case llvm::CmpInst::ICMP_UGT:
    case llvm::CmpInst::ICMP_SGT:
    case llvm::CmpInst::FCMP_UGT:
    case llvm::CmpInst::FCMP_OGT:
        func->createExpr(value, std::make_unique<CmpExpr>(val0, val1, ">", false));
        break;
    case llvm::CmpInst::ICMP_UGE:
    case llvm::CmpInst::ICMP_SGE:
    case llvm::CmpInst::FCMP_OGE:
    case llvm::CmpInst::FCMP_UGE:
        func->createExpr(value, std::make_unique<CmpExpr>(val0, val1, ">=", false));
        break;
    case llvm::CmpInst::ICMP_ULT:
    case llvm::CmpInst::ICMP_SLT:
    case llvm::CmpInst::FCMP_OLT:
    case llvm::CmpInst::FCMP_ULT:
        func->createExpr(value, std::make_unique<CmpExpr>(val0, val1, "<", false));
        break;
    case llvm::CmpInst::ICMP_ULE:
    case llvm::CmpInst::ICMP_SLE:
    case llvm::CmpInst::FCMP_OLE:
    case llvm::CmpInst::FCMP_ULE:
        func->createExpr(value, std::make_unique<CmpExpr>(val0, val1, "<=", false));
        break;
    case llvm::CmpInst::FCMP_FALSE:
        func->createExpr(value, std::make_unique<Value>("0", std::make_unique<IntegerType>("int", false)));
        break;
    case llvm::CmpInst::FCMP_TRUE:
        func->createExpr(value, std::make_unique<Value>("1", std::make_unique<IntegerType>("int", false)));
        break;
    default:
        throw std::invalid_argument("FCMP ORD/UNO and BAD PREDICATE not supported!");

    }
}


static void parseStoreInstruction(const llvm::Instruction& ins, bool isConstExpr, const llvm::Value* val, Func* func, Block* block) {
    auto type = func->getType(ins.getOperand(0)->getType());
    if (dynamic_cast<PointerType*>(type.get())) {
        if (llvm::Function* function = llvm::dyn_cast<llvm::Function>(ins.getOperand(0))) {
            if (!func->getExpr(ins.getOperand(0))) {
                func->createExpr(ins.getOperand(0), std::make_unique<Value>("&" + function->getName().str(), std::make_unique<VoidType>()));
            }
        }
    }

    //skip stacksave
    if (llvm::CallInst* CI = llvm::dyn_cast<llvm::CallInst>(ins.getOperand(0))) {
        if (CI->getCalledFunction()) {
            if (CI->getCalledFunction()->getName().str().compare("llvm.stacksave") == 0) {
                return;
            }
        }
    }

    llvm::Value* inst = ins.getOperand(0);
    bool isCast = false;
    //get rid of casts in case of asm output parameter
    while (llvm::CastInst* CI = llvm::dyn_cast<llvm::CastInst>(inst)) {
        inst = CI->getOperand(0);
        isCast = true;
    }

    if (isCast) {
        //inline asm with multiple outputs with casts
        if (llvm::ExtractValueInst* EVI = llvm::dyn_cast<llvm::ExtractValueInst>(inst)) {
            if (!func->getExpr(ins.getOperand(1))) {
                createConstantValue(ins.getOperand(1), func, block);
            }
            Expr* value = func->getExpr(ins.getOperand(1));
            Expr* asmExpr = func->getExpr(EVI->getOperand(0));

            if (auto RE = dynamic_cast<RefExpr*>(value)) {
                value = RE->expr;
            }

            if (auto AE = dynamic_cast<AsmExpr*>(asmExpr)) {
                AE->addOutputExpr(value, EVI->getIndices()[0]);
                return;
            }
        }

        //inline asm with single output with cast
        if (AsmExpr* AE = dynamic_cast<AsmExpr*>(func->getExpr(inst))) {
            if (!func->getExpr(ins.getOperand(1))) {
                createConstantValue(ins.getOperand(1), func, block);
            }
            Expr* value = func->getExpr(ins.getOperand(1));

            if (auto RE = dynamic_cast<RefExpr*>(value)) {
                value = RE->expr;
            }

            AE->addOutputExpr(value, 0);
            return;
        }
    }

    //inline asm with multiple outputs
    if (llvm::ExtractValueInst* EVI = llvm::dyn_cast<llvm::ExtractValueInst>(ins.getOperand(0))) {
        if (!func->getExpr(ins.getOperand(1))) {
            createConstantValue(ins.getOperand(1), func, block);
        }
        Expr* value = func->getExpr(ins.getOperand(1));
        Expr* asmExpr = func->getExpr(EVI->getOperand(0));

        if (auto RE = dynamic_cast<RefExpr*>(value)) {
            value = RE->expr;
        }

        if (auto AE = dynamic_cast<AsmExpr*>(asmExpr)) {
            AE->addOutputExpr(value, EVI->getIndices()[0]);
            return;
        }
    }

    if (!func->getExpr(ins.getOperand(0))) {
        createConstantValue(ins.getOperand(0), func, block);
    }
    Expr* val0 = func->getExpr(ins.getOperand(0));

    if (!func->getExpr(ins.getOperand(1))) {
        createConstantValue(ins.getOperand(1), func, block);
    }
    Expr* val1 = func->getExpr(ins.getOperand(1));

    //storing to NULL
    if (val1->toString().compare("0") == 0) {
        auto newCast = std::make_unique<CastExpr>(val1, func->getType(ins.getOperand(1)->getType()));
        val1 = newCast.get();
        block->casts.push_back(std::move(newCast));
    }

    if (block->derefs.find(val1) == block->derefs.end()) {
        block->derefs[val1] = std::make_unique<DerefExpr>(val1);
    }

    //inline asm with single output
    if (auto AE = dynamic_cast<AsmExpr*>(val0)) {
        if (auto RE = dynamic_cast<RefExpr*>(val1)) {
            val1 = RE->expr;
        }
        AE->addOutputExpr(val1, 0);
        return;
    }
    auto v = isConstExpr ? val : &ins;
    auto assign = std::make_unique<AssignExpr>(block->derefs[val1].get(), val0);

    if (!isConstExpr) {
        block->addExpr(assign.get());
    }
    func->createExpr(v, std::move(assign));
}


static void parsePhiInstruction(const llvm::Instruction& ins, bool isConstExpr, const llvm::Value *val, Func* func, Block* block) {
    const llvm::Value* value = isConstExpr ? val : &ins;
    const auto* phi = llvm::cast<const llvm::PHINode>(&ins);
    assert(phi != nullptr && "instruction is not a phi node or is null");

    // create variable for phi
    func->createPhiVariable(value);

    // for all incoming blocks:
    for (auto i = 0; i < phi->getNumIncomingValues(); ++i) {
        auto* inBlock = phi->getIncomingBlock(i);
        auto* inValue = phi->getIncomingValue(i);

        if (!func->getExpr(inValue)) {
            createConstantValue(inValue, func, block);
        }

        // at the end of @inBlock (just before br instruction), append an assignment @value = @inValue
        auto* myBlock = func->getBlock(inBlock);
        myBlock->addPhiAssignment(std::make_unique<AssignExpr>(func->getExpr(value), func->getExpr(inValue)));
    }
}

static void parseBrInstruction(const llvm::Instruction& ins, bool isConstExpr, const llvm::Value* val, Func* func, Block* block) {
    const llvm::Value* value = isConstExpr ? val : &ins;

    //no condition
    if (ins.getNumOperands() == 1) {
        std::string trueBlock = func->createBlockIfNotExist((llvm::BasicBlock*)ins.getOperand(0))->blockName;
        func->createExpr(value, std::make_unique<IfExpr>(trueBlock));

        if (!isConstExpr) {
            block->addExpr(func->getExpr(&ins));
        }
        return;
    }

    Expr* cmp = func->getExpr(ins.getOperand(0));

    std::string falseBlock = func->createBlockIfNotExist((llvm::BasicBlock*)ins.getOperand(1))->blockName;
    std::string trueBlock = func->createBlockIfNotExist((llvm::BasicBlock*)ins.getOperand(2))->blockName;

    func->createExpr(value, std::make_unique<IfExpr>(cmp, trueBlock, falseBlock));

    if (!isConstExpr) {
        block->addExpr(func->getExpr(&ins));
    }
}

static void parseLoadInstruction(const llvm::Instruction& ins, bool isConstExpr, const llvm::Value* val, Func* func, Block* block) {
    auto* v = isConstExpr ? val : &ins;

    if (!func->getExpr(ins.getOperand(0))) {
        createConstantValue(ins.getOperand(0), func, block);
    }

    //create new variable for every load instruction
    auto deref = std::make_unique<DerefExpr>(func->getExpr(ins.getOperand(0)));
    auto var = std::make_unique<Value>(func->getVarName(), deref->getType()->clone());
    auto assign = std::make_unique<AssignExpr>(var.get(), deref.get());

    block->addExpr(var.get());
    block->addExpr(assign.get());

    func->createExpr(v, std::move(var));

    block->loadDerefs.push_back(std::move(deref));
    block->stores.push_back(std::move(assign));
}

static void parseBinaryInstruction(const llvm::Instruction& ins, bool isConstExpr, const llvm::Value* val, Func* func, Block* block) {
    if (!func->getExpr(ins.getOperand(0))) {
        createConstantValue(ins.getOperand(0), func, block);
    }
    Expr* val0 = func->getExpr(ins.getOperand(0));

    if (!func->getExpr(ins.getOperand(1))) {
        createConstantValue(ins.getOperand(1), func, block);
    }
    Expr* val1 = func->getExpr(ins.getOperand(1));

    const llvm::Value* value = isConstExpr ? val : &ins;

    std::unique_ptr<Expr> expr;
    switch (ins.getOpcode()) {
    case llvm::Instruction::Add:
    case llvm::Instruction::FAdd:
        expr = std::make_unique<AddExpr>(val0, val1);
        break;
    case llvm::Instruction::Sub:
    case llvm::Instruction::FSub:
        expr = std::make_unique<SubExpr>(val0, val1);
        break;
    case llvm::Instruction::Mul:
    case llvm::Instruction::FMul:
        expr = std::make_unique<MulExpr>(val0, val1);
        break;
    case llvm::Instruction::SDiv:
    case llvm::Instruction::UDiv:
    case llvm::Instruction::FDiv:
        expr = std::make_unique<DivExpr>(val0, val1);
        break;
    case llvm::Instruction::SRem:
    case llvm::Instruction::URem:
    case llvm::Instruction::FRem:
        expr = std::make_unique<RemExpr>(val0, val1);
        break;
    case llvm::Instruction::And:
        expr = std::make_unique<AndExpr>(val0, val1);
        break;
    case llvm::Instruction::Or:
        expr = std::make_unique<OrExpr>(val0, val1);
        break;
    case llvm::Instruction::Xor:
        expr = std::make_unique<XorExpr>(val0, val1);
        break;
    default:
        llvm::outs() << "Unsupported binary instruction: " << ins << "\n";
        throw std::invalid_argument("Unsupported binary instruction encountered!");
    }

    func->createExpr(value, std::move(expr));
}

static void parseRetInstruction(const llvm::Instruction& ins, bool isConstExpr, const llvm::Value* val, Func* func, Block* block) {
    const llvm::Value* value = isConstExpr ? val : &ins;

    if (ins.getNumOperands() == 0) {
        func->createExpr(value, std::make_unique<RetExpr>());
    } else {
        if (func->getExpr(ins.getOperand(0)) == nullptr) {
            createConstantValue(ins.getOperand(0), func, block);
        }
        Expr* expr = func->getExpr(ins.getOperand(0));

        func->createExpr(value, std::make_unique<RetExpr>(expr));
    }

    block->addExpr(func->getExpr(&ins));
}

static void parseSwitchInstruction(const llvm::Instruction& ins, bool isConstExpr, const llvm::Value* val, Func* func, Block* block) {
    std::map<int, std::string> cases;

    if (!func->getExpr(ins.getOperand(0))) {
        createConstantValue(ins.getOperand(0), func, block);
    }
    Expr* cmp = func->getExpr(ins.getOperand(0));

    std::string def = func->createBlockIfNotExist(llvm::cast<llvm::BasicBlock>(ins.getOperand(1)))->blockName;
    const llvm::SwitchInst* switchIns = llvm::cast<const llvm::SwitchInst>(&ins);

    for (const auto& switchCase : switchIns->cases()) {
        CaseHandle caseHandle = static_cast<CaseHandle>(&switchCase);
        cases[caseHandle->getCaseValue()->getSExtValue()] = func->createBlockIfNotExist(caseHandle->getCaseSuccessor())->blockName;
    }

    if (!isConstExpr) {
        func->createExpr(&ins, std::make_unique<SwitchExpr>(cmp, def, cases));
        block->addExpr(func->getExpr(&ins));
    } else {
        func->createExpr(val, std::make_unique<SwitchExpr>(cmp, def, cases));
    }
}

static void parseAsmInst(const llvm::Instruction& ins, bool isConstExpr, const llvm::Value* val, Func* func, Block* block) {
    std::string inst;

    switch(ins.getOpcode()) {
    case llvm::Instruction::Unreachable:
        inst = "int3";
        break;
    case llvm::Instruction::Fence:
        inst = "fence";
        break;
    default:
        break;
    }

    if (!isConstExpr) {
        func->createExpr(&ins, std::make_unique<AsmExpr>(inst, std::vector<std::pair<std::string, Expr*>>(), std::vector<std::pair<std::string, Expr*>>(), ""));
        block->addExpr(func->getExpr(&ins));
    } else {
        func->createExpr(val, std::make_unique<AsmExpr>(inst, std::vector<std::pair<std::string, Expr*>>(), std::vector<std::pair<std::string, Expr*>>(), ""));
    }
}

static void parseShiftInstruction(const llvm::Instruction& ins, bool isConstExpr, const llvm::Value* val, Func* func, Block* block) {
    if (!func->getExpr(ins.getOperand(0))) {
        createConstantValue(ins.getOperand(0), func, block);
    }
    Expr* val0 = func->getExpr(ins.getOperand(0));

    if (!func->getExpr(ins.getOperand(1))) {
        createConstantValue(ins.getOperand(1), func, block);
    }
    Expr* val1 = func->getExpr(ins.getOperand(1));

    const llvm::Value* value = isConstExpr ? val : &ins;

    switch (ins.getOpcode()) {
    case llvm::Instruction::Shl:
        func->createExpr(value, std::make_unique<ShlExpr>(val0, val1));
        break;
    case llvm::Instruction::LShr:
        func->createExpr(value, std::make_unique<LshrExpr>(val0, val1));
        break;
    case llvm::Instruction::AShr:
        func->createExpr(value, std::make_unique<AshrExpr>(val0, val1));
        break;
    }
}
std::string getRegisterString(const std::string& str) {
    std::string reg = str;
    std::string ret;

    if (reg[0] != '{') {
        return reg;
    }

    if (reg[2] == 'i') {
        ret += (char)std::toupper(reg[1]);
    }

    if (reg[2] == 'x') {
        ret += reg[1];
    }

    return ret;
}

std::vector<std::string> getAsmInputStrings(llvm::InlineAsm::ConstraintInfoVector info) {
    std::vector<std::string> ret;

    for (llvm::InlineAsm::ConstraintInfoVector::iterator iter = info.begin(); iter != info.end(); iter++) {
        if (iter->Type != llvm::InlineAsm::isInput) {
            continue;
        }

        std::string input;
        for (unsigned i = 0; i < iter->Codes.size(); i++) {
            input += getRegisterString(iter->Codes[i]);
        }

        if (iter->isEarlyClobber) {
            input = "&" + input;
        }

        if (iter->isIndirect) {
            input = "*" + input;
        }

        ret.push_back("\"" + input + "\"");
    }

    return ret;
}

std::string getAsmUsedRegString(llvm::InlineAsm::ConstraintInfoVector info) {
    static std::vector<std::string> CLOBBER = {"memory", "rax", "eax", "ax", "al", "rbx", "ebx", "bx", "bl", "rcx", "ecx", "cx",
                                               "cl", "rdx", "edx", "dx", "dl", "rsi", "esi", "si", "rdi", "edi", "di"};

    std::string ret;
    bool first = true;

    for (llvm::InlineAsm::ConstraintInfoVector::iterator iter = info.begin(); iter != info.end(); iter++) {
        if (iter->Type != llvm::InlineAsm::isClobber) {
            continue;
        }

        std::string clobber = iter->Codes[0];
        clobber = clobber.substr(1, clobber.size() - 2);

        if (std::find(CLOBBER.begin(), CLOBBER.end(), clobber) != CLOBBER.end()) {
            if (!first) {
                ret += ", ";
            }
            first = false;

            ret += "\"";
            if (clobber.compare("memory") != 0) {
                ret += "%";
            }
            ret += clobber + "\"";
        }
    }

    return ret;
}


std::vector<std::string> getAsmOutputStrings(llvm::InlineAsm::ConstraintInfoVector info) {
    std::vector<std::string> ret;

    for (llvm::InlineAsm::ConstraintInfoVector::iterator iter = info.begin(); iter != info.end(); iter++) {
        if (iter->Type != llvm::InlineAsm::isOutput) {
            continue;
        }

        std::string output;
        if (iter->isMultipleAlternative) {
            bool first = true;
            for (unsigned i = 0; i < iter->multipleAlternatives.size(); i++) {
                if (!first) {
                    output += ",";
                }

                if (iter->multipleAlternatives[i].Codes.size() > 1) {
                    output += iter->multipleAlternatives[i].Codes[0];
                    output += getRegisterString(iter->multipleAlternatives[i].Codes[1]);
                } else {
                    output += getRegisterString(iter->multipleAlternatives[i].Codes[0]);
                }

                first = false;
            }

            if (iter->isEarlyClobber) {
                output = "&" + output;
            }

            if (iter->isIndirect) {
                ret.push_back("\"=*" + output + "\"");
            } else {
                ret.push_back("\"=" + output + "\"");
            }
            continue;
        }

        if (iter->Codes.size() > 1) {
            output = iter->Codes[0];
            output += getRegisterString(iter->Codes[1]);
        } else {
            output = getRegisterString(iter->Codes[0]);
        }

        if (iter->isEarlyClobber) {
            output = "&" + output;
        }

        if (iter->isIndirect) {
            ret.push_back("\"=*" + output + "\"");
        } else {
            ret.push_back("\"=" + output + "\"");
        }
    }

    return ret;
}

static void parseCallInstruction(const llvm::Instruction& ins, bool isConstExpr, const llvm::Value* val, Func* func, Block* block) {
    const llvm::Value* value = isConstExpr ? val : &ins;
    const llvm::CallInst* callInst = llvm::cast<llvm::CallInst>(&ins);
    Expr* funcValue = nullptr;
    std::string funcName;
    std::vector<Expr*> params;
    std::unique_ptr<Type> type = nullptr;

    if (callInst->getCalledFunction()) {
        funcName = callInst->getCalledFunction()->getName().str();
        type = func->getType(callInst->getCalledFunction()->getReturnType());

        if (funcName.compare("llvm.dbg.declare") == 0) {
            return;
        }

        if (funcName.compare("llvm.trap") == 0 || funcName.compare("llvm.debugtrap") == 0) {
            func->createExpr(&ins, std::make_unique<AsmExpr>("int3", std::vector<std::pair<std::string, Expr*>>(), std::vector<std::pair<std::string, Expr*>>(), ""));
            block->addExpr(func->getExpr(&ins));
            return;
        }

        if (funcName.compare("llvm.stacksave") == 0 || funcName.compare("llvm.stackrestore") == 0) {
            func->stackIgnored();
            return;
        }

        if (funcName.substr(0,4).compare("llvm") == 0) {
            if (isCFunc(trimPrefix(funcName))) {
                funcName = trimPrefix(funcName);
            } else {
                std::replace(funcName.begin(), funcName.end(), '.', '_');
            }
        }
    } else {
        llvm::PointerType* PT = llvm::cast<llvm::PointerType>(callInst->getCalledValue()->getType());
        llvm::FunctionType* FT = llvm::cast<llvm::FunctionType>(PT->getPointerElementType());
        type = func->getType(FT->getReturnType());

        if (llvm::isa<llvm::InlineAsm>(callInst->getCalledValue())) {
            parseInlineASM(ins, func, block);
            return;
        }

        if (!func->getExpr(callInst->getCalledValue())) {
            createConstantValue(callInst->getCalledValue(), func, block);
        }

        if (!func->getExpr(callInst->getCalledValue())) {
            createConstantValue(callInst->getCalledValue(), func, block);
        }
        funcValue = func->getExpr(callInst->getCalledValue());
    }

    int i = 0;
    for (const llvm::Use& param : callInst->arg_operands()) {
        //sometimes LLVM uses these functions with more arguments than their C counterparts
        if ((funcName.compare("memcpy") == 0 || funcName.compare("memmove") == 0 || funcName.compare("memset") == 0)  && i == 3) {
            break;
        }

        if (!func->getExpr(param)) {
            createFuncCallParam(param, func, block);
        }
        params.push_back(func->getExpr(param));
        i++;
    }

    if (funcName.compare("va_start") == 0) {
        params.push_back(func->lastArg);
    }

    //call function if it returns void, otherwise store function return value to a new variable and use this variable instead of function call
    if (dynamic_cast<VoidType*>(type.get())) {
        func->createExpr(value, std::make_unique<CallExpr>(funcValue, funcName, params, type->clone()));

        if (!isConstExpr) {
            block->addExpr(func->getExpr(&ins));
        }
    } else {
        block->callExprMap.push_back(std::make_unique<CallExpr>(funcValue, funcName, params, type->clone()));

        func->createExpr(value, std::make_unique<Value>(func->getVarName(), type->clone()));
        block->callValueMap.push_back(std::make_unique<AssignExpr>(func->getExpr(value), block->callExprMap[block->callExprMap.size() - 1].get()));

        if (!isConstExpr) {
            block->addExpr(func->getExpr(&ins));
            block->addExpr(block->callValueMap[block->callValueMap.size() - 1].get());
        }
    }
}

static std::string toRawString(const std::string& str) {
    std::string ret = str;
    size_t pos = 0;

    while ((pos = ret.find("%", pos)) != std::string::npos) {
        if (ret[pos + 1] < 48 || ret[pos + 1] > 57) {
            ret.replace(pos, 1, "%%");
            pos += 2;
        }
    }

    pos = 0;
    while ((pos = ret.find("$", pos)) != std::string::npos) {
        if (ret[pos + 1] != '$') {
            ret.replace(pos, 1, "%");
        } else {
            pos += 2;
        }
    }

    pos = 0;
    while ((pos = ret.find("$$", pos)) != std::string::npos) {
        ret.replace(pos, 2, "$");
    }

    pos = 0;
    while ((pos = ret.find("\"", pos)) != std::string::npos) {
        ret.replace(pos, 1, "\\\"");
        pos += 2;
    }

    pos = 0;
    while ((pos = ret.find("\n", pos)) != std::string::npos) {
        ret.replace(pos, 1, "\\n");
        pos += 2;
    }

    pos = 0;
    while ((pos = ret.find("\t", pos)) != std::string::npos) {
        ret.replace(pos, 1, "\\t");
        pos += 2;
    }

    return ret;
}

static void parseInlineASM(const llvm::Instruction& ins, Func* func, Block* block) {
    const auto callInst = llvm::cast<llvm::CallInst>(&ins);
    const auto IA = llvm::cast<llvm::InlineAsm>(callInst->getCalledValue());

    std::string asmString = toRawString(IA->getAsmString());
    std::vector<std::string> inputStrings;
    std::vector<std::string> outputStrings;
    std::string usedReg;

    if (!IA->getConstraintString().empty()) {
        outputStrings = getAsmOutputStrings(IA->ParseConstraints());
        inputStrings = getAsmInputStrings(IA->ParseConstraints());
        usedReg = getAsmUsedRegString(IA->ParseConstraints());
    }

    std::vector<Expr*> args;
    for (const llvm::Use& arg : callInst->arg_operands()) {
        if (!func->getExpr(arg.get())) {
            createFuncCallParam(arg, func, block);
        }

        const llvm::AllocaInst* AI = llvm::dyn_cast<llvm::AllocaInst>(arg.get());
        const llvm::GetElementPtrInst* GI = llvm::dyn_cast<llvm::GetElementPtrInst>(arg.get());
        const llvm::CastInst* CI = llvm::dyn_cast<llvm::CastInst>(arg.get());
        const llvm::GlobalVariable* GV = llvm::dyn_cast<llvm::GlobalVariable>(arg.get());
        llvm::ConstantExpr* CE = llvm::dyn_cast<llvm::ConstantExpr>(arg.get());

        //creates new variable for every alloca, getelementptr and cast instruction and global variable that inline asm takes as a parameter
        //as inline asm has problem with casts and expressions containing "&" symbol
        if (GI || CI || AI || GV) {
            block->vars.push_back(std::make_unique<Value>(func->getVarName(), func->getExpr(arg.get())->getType()->clone()));
            block->stores.push_back(std::make_unique<AssignExpr>(block->vars[block->vars.size() - 1].get(), func->getExpr(arg.get())));
            args.push_back(block->vars[block->vars.size() - 1].get());

            block->addExpr(block->vars[block->vars.size() - 1].get());
            block->addExpr(block->stores[block->stores.size() - 1].get());
        } else if (CE) {
            if (llvm::isa<llvm::GetElementPtrInst>(CE->getAsInstruction())) {
                block->vars.push_back(std::make_unique<Value>(func->getVarName(), func->getExpr(arg.get())->getType()->clone()));
                block->stores.push_back(std::make_unique<AssignExpr>(block->vars[block->vars.size() - 1].get(), func->getExpr(arg.get())));
                args.push_back(block->vars[block->vars.size() - 1].get());

                block->addExpr(block->vars[block->vars.size() - 1].get());
                block->addExpr(block->stores[block->stores.size() - 1].get());
            } else {
                args.push_back(func->getExpr(arg.get()));
            }
        } else {
            args.push_back(func->getExpr(arg.get()));
        }
    }

    //output parsing
    std::vector<std::pair<std::string, Expr*>> output;
    Expr* expr = nullptr;
    unsigned pos = 0;
    for (const auto& str : outputStrings) {
        if (str.find('*') == std::string::npos) {
            output.push_back({str, expr});
        } else {
            output.push_back({str, args[pos]});
            pos++;
        }
    }

    //input parsing
    std::vector<std::pair<std::string, Expr*>> input;
    unsigned arg = args.size() - 1;
    for (int i = inputStrings.size() - 1; i >= 0; i--) {
        //handle number constraint reffering to multiple option constraint
        std::string inputString = inputStrings[i].substr(1, inputStrings[i].size() - 2);
        std::string::const_iterator it = inputString.begin();
        while (it != inputString.end() && std::isdigit(*it)) {
            ++it;
        }

        //handle number constraint refering to multi-alternative output constraint
        if (it == inputString.end()) {
            int num = std::stoi(inputString);
            std::string outputString = output[num].first;
            if (outputString.find(',') != std::string::npos) {
                output[num].first = "\"+" + outputString.substr(3, outputString.size() - 3);
                arg--;
                continue;
            }
        }

        input.push_back({inputStrings[i], args[arg]});
        arg--;
    }

    func->createExpr(&ins, std::make_unique<AsmExpr>(asmString, output, input, usedReg));
    block->addExpr(func->getExpr(&ins));
}

static void parseCastInstruction(const llvm::Instruction& ins, bool isConstExpr, const llvm::Value* val, Func* func, Block* block) {
    if (func->getExpr(ins.getOperand(0)) == nullptr) {
        createConstantValue(ins.getOperand(0), func, block);
    }
    Expr* expr = func->getExpr(ins.getOperand(0));

    auto AE = dynamic_cast<AsmExpr*>(expr);
    //operand is used for initializing output in inline asm
    if (!expr || AE) {
        return;
    }

    const llvm::CastInst* CI = llvm::cast<const llvm::CastInst>(&ins);

    func->createExpr(isConstExpr ? val : &ins, std::make_unique<CastExpr>(expr, func->getType(CI->getDestTy())));

    if (ins.getOpcode() == llvm::Instruction::FPToUI) {
        static_cast<IntegerType*>(func->getExpr(isConstExpr ? val : &ins)->getType())->unsignedType = true;
    }
}

static void parseSelectInstruction(const llvm::Instruction& ins, bool isConstExpr, const llvm::Value* val, Func* func, Block* block) {
    const llvm::SelectInst* SI = llvm::cast<const llvm::SelectInst>(&ins);
    Expr* cond = func->getExpr(SI->getCondition());

    if (!func->getExpr(ins.getOperand(1))) {
        createConstantValue(ins.getOperand(1), func, block);
    }
    Expr* val0 = func->getExpr(ins.getOperand(1));

    if (!func->getExpr(ins.getOperand(2))) {
        createConstantValue(ins.getOperand(2), func, block);
    }
    Expr* val1 = func->getExpr(ins.getOperand(2));

    const llvm::Value* value = isConstExpr ? val : &ins;
    func->createExpr(value, std::make_unique<SelectExpr>(cond, val0, val1));
}

static void parseGepInstruction(const llvm::Instruction& ins, bool isConstExpr, const llvm::Value* val, Func* func, Block* block) {
    const llvm::GetElementPtrInst* gepInst = llvm::cast<llvm::GetElementPtrInst>(&ins);

    if (!func->getExpr(gepInst->getOperand(0))) {
        createConstantValue(gepInst->getOperand(0), func, block);
    }
    Expr* expr = func->getExpr(gepInst->getOperand(0));

    llvm::Type* prevType = gepInst->getOperand(0)->getType();
    Expr* prevExpr = expr;
    std::vector<std::unique_ptr<Expr>> indices;

    //if getelementptr contains null, cast it to given type
    if (expr->toString().compare("0") == 0) {
        block->casts.push_back(std::make_unique<CastExpr>(expr, func->getType(prevType)));
        prevExpr = block->casts[block->casts.size() - 1].get();
    }

    for (auto it = llvm::gep_type_begin(gepInst); it != llvm::gep_type_end(gepInst); it++) {
        if (!func->getExpr(it.getOperand())) {
            createConstantValue(it.getOperand(), func, block);
        }
        Expr* index = func->getExpr(it.getOperand());

        if (prevType->isPointerTy()) {
            if (index->toString().compare("0") == 0) {
                indices.push_back(std::make_unique<DerefExpr>(prevExpr));
            } else {
                indices.push_back(std::make_unique<PointerShift>(func->getType(prevType), prevExpr, index));
            }
        }

        if (prevType->isArrayTy()) {
            indices.push_back(std::make_unique<ArrayElement>(prevExpr, index, func->getType(prevType->getArrayElementType())));
        }

        if (prevType->isStructTy()) {
            llvm::ConstantInt* CI = llvm::dyn_cast<llvm::ConstantInt>(it.getOperand());
            if (!CI) {
                throw std::invalid_argument("Invalid GEP index - access to struct element only allows integer!");
            }

            indices.push_back(std::make_unique<StructElement>(func->getStruct(llvm::cast<llvm::StructType>(prevType)), prevExpr, CI->getSExtValue()));
        }

        prevType = it.getIndexedType();
        prevExpr = indices[indices.size() - 1].get();
    }
    block->geps.push_back(std::make_unique<GepExpr>(indices));
    func->createExpr(isConstExpr ? val : &ins, std::make_unique<RefExpr>(block->geps[block->geps.size() -1].get()));
}

static void parseLLVMInstruction(const llvm::Instruction& ins, bool isConstExpr, const llvm::Value* val, Func* func, Block *block) {
    switch (ins.getOpcode()) {
    case llvm::Instruction::Add:
    case llvm::Instruction::FAdd:
    case llvm::Instruction::Sub:
    case llvm::Instruction::FSub:
    case llvm::Instruction::Mul:
    case llvm::Instruction::FMul:
    case llvm::Instruction::UDiv:
    case llvm::Instruction::FDiv:
    case llvm::Instruction::SDiv:
    case llvm::Instruction::URem:
    case llvm::Instruction::FRem:
    case llvm::Instruction::SRem:
    case llvm::Instruction::And:
    case llvm::Instruction::Or:
    case llvm::Instruction::Xor:
        parseBinaryInstruction(ins, isConstExpr, val, func, block);
        break;
    case llvm::Instruction::Load:
        parseLoadInstruction(ins, isConstExpr, val, func, block);
        break;
    case llvm::Instruction::Store:
        parseStoreInstruction(ins, isConstExpr, val, func, block);
        break;
    case llvm::Instruction::ICmp:
    case llvm::Instruction::FCmp:
        parseCmpInstruction(ins, isConstExpr, val, func, block);
        break;
    case llvm::Instruction::Br:
        parseBrInstruction(ins, isConstExpr, val, func, block);
        break;
    case llvm::Instruction::Ret:
        parseRetInstruction(ins, isConstExpr, val, func, block);
        break;
    case llvm::Instruction::PHI:
        parsePhiInstruction(ins, isConstExpr, val, func, block);
        break;
    case llvm::Instruction::Switch:
        parseSwitchInstruction(ins, isConstExpr, val, func, block);
        break;
    case llvm::Instruction::Unreachable:
    case llvm::Instruction::Fence:
        parseAsmInst(ins, isConstExpr, val, func, block);
        break;
    case llvm::Instruction::Shl:
    case llvm::Instruction::LShr:
    case llvm::Instruction::AShr:
        parseShiftInstruction(ins, isConstExpr, val, func, block);
        break;
    case llvm::Instruction::Call:
        parseCallInstruction(ins, isConstExpr, val, func, block);
        break;
    case llvm::Instruction::SExt:
    case llvm::Instruction::ZExt:
    case llvm::Instruction::FPToSI:
    case llvm::Instruction::SIToFP:
    case llvm::Instruction::FPTrunc:
    case llvm::Instruction::FPExt:
    case llvm::Instruction::FPToUI:
    case llvm::Instruction::UIToFP:
    case llvm::Instruction::PtrToInt:
    case llvm::Instruction::IntToPtr:
    case llvm::Instruction::Trunc:
    case llvm::Instruction::BitCast:
        parseCastInstruction(ins, isConstExpr, val, func, block);
        break;
    case llvm::Instruction::Select:
        parseSelectInstruction(ins, isConstExpr, val, func, block);
        break;
    case llvm::Instruction::GetElementPtr:
        parseGepInstruction(ins, isConstExpr, val, func, block);
        break;
    case llvm::Instruction::ExtractValue:
        parseExtractValueInstruction(ins, isConstExpr, val, func, block);
        break;
    case llvm::Instruction::Alloca:
        llvm::outs() << "Alloca instructions should be removed by now!\n";
        llvm::outs() << "But this instruction was found: '" << ins << "'\n";
        throw std::invalid_argument("Alloca instructions should be removed by now!");
        break;
    default:
        llvm::outs() << "File contains unsupported instruction!\n";
        llvm::outs() << ins << "\n";
        throw std::invalid_argument("");
        break;
    }
}


void createExpressions(const llvm::Module* module, Program& program) {
    for (const auto& function : module->functions()) {
        auto* func = program.getFunction(&function);
        for (const auto& block : function) {
            auto* myBlock = func->getBlock(&block);

            for (const auto& ins : block) {
                if (ins.getOpcode() != llvm::Instruction::Alloca) {
                    parseLLVMInstruction(ins, false, nullptr, func, myBlock);
                } else {
                    // TODO what exactly is this for?
                    func->createExpr(&ins, std::make_unique<RefExpr>(myBlock->getValue(&ins)));
                }
            }

        }
    }
}
