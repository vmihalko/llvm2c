#include <llvm/IR/Instruction.h>

bool isIntegerCompareUnsigned(const llvm::CmpInst* cmp) {
    switch (cmp->getPredicate()) {
    case llvm::CmpInst::ICMP_EQ:
    case llvm::CmpInst::FCMP_OEQ:
    case llvm::CmpInst::FCMP_UEQ:
        return true;
    case llvm::CmpInst::ICMP_NE:
    case llvm::CmpInst::FCMP_ONE:
    case llvm::CmpInst::FCMP_UNE:
        return true;

    case llvm::CmpInst::ICMP_UGT:
    case llvm::CmpInst::ICMP_UGE:
    case llvm::CmpInst::ICMP_ULT:
    case llvm::CmpInst::ICMP_ULE:
        return true;

    case llvm::CmpInst::ICMP_SGT:
    case llvm::CmpInst::ICMP_SGE:
    case llvm::CmpInst::ICMP_SLT:
    case llvm::CmpInst::ICMP_SLE:
        return false;

    default:
        throw std::invalid_argument("unable to determine signedness of icmp predicate");
    }
}

std::string getComparePredicate(const llvm::CmpInst* cmp) {
    switch(cmp->getPredicate()) {
    case llvm::CmpInst::ICMP_EQ:
    case llvm::CmpInst::FCMP_OEQ:
    case llvm::CmpInst::FCMP_UEQ:
        return "==";
    case llvm::CmpInst::ICMP_NE:
    case llvm::CmpInst::FCMP_ONE:
    case llvm::CmpInst::FCMP_UNE:
        return "!=";
    case llvm::CmpInst::ICMP_UGT:
    case llvm::CmpInst::ICMP_SGT:
    case llvm::CmpInst::FCMP_UGT:
    case llvm::CmpInst::FCMP_OGT:
        return ">";
    case llvm::CmpInst::ICMP_UGE:
    case llvm::CmpInst::ICMP_SGE:
    case llvm::CmpInst::FCMP_OGE:
    case llvm::CmpInst::FCMP_UGE:
        return ">=";
    case llvm::CmpInst::ICMP_ULT:
    case llvm::CmpInst::ICMP_SLT:
    case llvm::CmpInst::FCMP_OLT:
    case llvm::CmpInst::FCMP_ULT:
        return "<";
    case llvm::CmpInst::ICMP_ULE:
    case llvm::CmpInst::ICMP_SLE:
    case llvm::CmpInst::FCMP_OLE:
    case llvm::CmpInst::FCMP_ULE:
        return "<=";
    default:
        throw std::invalid_argument("FCMP ORD/UNO and BAD PREDICATE not supported!");

    }
}
