#include <llvm/IR/Instructions.h>

llvm::CmpInst* negateCmpInst(llvm::CmpInst* cmpInst) {
    llvm::CmpInst::Predicate originalPredicate = cmpInst->getPredicate();
    llvm::CmpInst::Predicate negatedPredicate;

    // Switch case to determine the negated predicate based on the original predicate
    switch (originalPredicate) {
    case llvm::CmpInst::ICMP_EQ:
        negatedPredicate = llvm::CmpInst::ICMP_NE;
        break;
    case llvm::CmpInst::FCMP_OEQ:
        negatedPredicate = llvm::CmpInst::FCMP_ONE;
        break;
    case llvm::CmpInst::FCMP_UEQ:
        negatedPredicate = llvm::CmpInst::FCMP_UNE;
        break;
    case llvm::CmpInst::ICMP_NE:
        negatedPredicate = llvm::CmpInst::ICMP_EQ;
        break;
    case llvm::CmpInst::FCMP_ONE:
        negatedPredicate = llvm::CmpInst::FCMP_OEQ;
        break;
    case llvm::CmpInst::FCMP_UNE:
        negatedPredicate = llvm::CmpInst::FCMP_UEQ;
        break;
    case llvm::CmpInst::ICMP_UGT:
        negatedPredicate = llvm::CmpInst::ICMP_ULE;
        break;
    case llvm::CmpInst::ICMP_SGT:
        negatedPredicate = llvm::CmpInst::ICMP_SLE;
        break;
    case llvm::CmpInst::FCMP_UGT:
        negatedPredicate = llvm::CmpInst::FCMP_ULE;
        break;
    case llvm::CmpInst::FCMP_OGT:
        negatedPredicate = llvm::CmpInst::FCMP_OLE;
        break;
    case llvm::CmpInst::ICMP_UGE:
        negatedPredicate = llvm::CmpInst::ICMP_ULT;
        break;
    case llvm::CmpInst::ICMP_SGE:
        negatedPredicate = llvm::CmpInst::ICMP_SLT;
        break;
    case llvm::CmpInst::FCMP_OGE:
        negatedPredicate = llvm::CmpInst::FCMP_OLT;
        break;
    case llvm::CmpInst::FCMP_UGE:
        negatedPredicate = llvm::CmpInst::FCMP_ULT;
        break;
    case llvm::CmpInst::ICMP_ULT:
        negatedPredicate = llvm::CmpInst::ICMP_UGE;
        break;
    case llvm::CmpInst::ICMP_SLT:
        negatedPredicate = llvm::CmpInst::ICMP_SGE;
        break;
    case llvm::CmpInst::FCMP_OLT:
        negatedPredicate = llvm::CmpInst::FCMP_OGE;
        break;
    case llvm::CmpInst::FCMP_ULT:
        negatedPredicate = llvm::CmpInst::FCMP_UGE;
        break;
    case llvm::CmpInst::ICMP_ULE:
        negatedPredicate = llvm::CmpInst::ICMP_UGT;
        break;
    case llvm::CmpInst::ICMP_SLE:
        negatedPredicate = llvm::CmpInst::ICMP_SGT;
        break;
    case llvm::CmpInst::FCMP_OLE:
        negatedPredicate = llvm::CmpInst::FCMP_OGT;
        break;
    case llvm::CmpInst::FCMP_ULE:
        negatedPredicate = llvm::CmpInst::FCMP_UGT;
        break;
    default:
        // Handle any unsupported predicate
        llvm::errs() << "Unsupported predicate\n";
        return nullptr;
    }

    // Create a new CmpInst with the negated predicate
    llvm::CmpInst* negatedCmpInst = llvm::CmpInst::Create(cmpInst->getOpcode(), negatedPredicate,
                                                          cmpInst->getOperand(0), cmpInst->getOperand(1),
                                                          cmpInst->getName() + ".negated", cmpInst);

    return negatedCmpInst;
}
