#include <llvm/IR/Instruction.h>
#include <memory>
#include <algorithm>
#include <cerrno>
#include <climits>
#include <cstdlib>
#include <cstring>

#include "../core/Program.h"
#include "../core/Func.h"
#include "../core/Block.h"

#include "../expr/ExprVisitor.h"

class SignCastsVisitor : public ExprVisitor {
    Block* block;
    Program& program;

    Expr* castIfNeeded(Expr* expr, bool isUnsigned);
    IntegerType* toggleSignedness(IntegerType* ty);
    void enforceCSignedness(BinaryExpr& expr, bool isUnsigned);
    void enforceShiftSignedness(BinaryExpr& expr, bool isUnsigned);

public:
    SignCastsVisitor(Program& program, Block* block) : block(block), program(program) {}

    void visit(AggregateElement& expr) override;
    void visit(ArrayElement& expr) override;
    void visit(ExtractValueExpr& expr) override;
    void visit(IfExpr& expr) override;
    void visit(SwitchExpr& expr) override;
    void visit(AsmExpr& expr) override;
    void visit(CallExpr& expr) override;
    void visit(PointerShift& expr) override;
    void visit(GepExpr& expr) override;
    void visit(SelectExpr& expr) override;
    void visit(RefExpr& expr) override;
    void visit(DerefExpr& expr) override;
    void visit(RetExpr& expr) override;
    void visit(CastExpr& expr) override;
    void visit(AddExpr& expr) override;
    void visit(SubExpr& expr) override;
    void visit(AssignExpr& expr) override;
    void visit(MulExpr& expr) override;
    void visit(DivExpr& expr) override;
    void visit(RemExpr& expr) override;
    void visit(AndExpr& expr) override;
    void visit(OrExpr& expr) override;
    void visit(XorExpr& expr) override;
    void visit(CmpExpr& expr) override;
    void visit(ShlExpr& expr) override;
    void visit(AshrExpr& expr) override;
    void visit(LshrExpr& expr) override;
    void visit(MinusExpr& expr) override;
    void visit(LogicalNot& expr) override;
    void visit(LogicalAnd& expr) override;
    void visit(LogicalOr& expr) override;
    void visit(ArrowExpr& expr) override;
    void visit(DoWhile& expr) override;
};


void addSignCasts(const llvm::Module* module, Program& program) {
    assert(program.isPassCompleted(PassType::CreateExpressions));
    for (const llvm::Function& func : module->functions()) {
        auto* function = program.getFunction(&func);
        for (const auto& block : func) {
            auto* myBlock = function->getBlock(&block);
            SignCastsVisitor scv(program, myBlock);

            for (auto it = myBlock->expressions.begin(); it != myBlock->expressions.end(); ++it) {
                auto expr = *it;
                expr->accept(scv);

            }
        }
    }

    program.addPass(PassType::AddSignCasts);
}

void SignCastsVisitor::visit(IfExpr& expr) {
    if (expr.cmp) {
        expr.cmp->accept(*this);
    }
}

// A do-while's condition comes from the loop-latch branch (a separate icmp from
// the header guard that IfExpr handles). Without traversing it here, a signed
// icmp on an unsigned operand keeps its raw unsigned form (e.g. `x >= 0`, which
// is always true), turning a terminating loop into an infinite one. Route the
// condition (and body, mirroring PropagateTypesVisitor) through the same
// CmpExpr/castIfNeeded path used for every other comparison.
void SignCastsVisitor::visit(DoWhile& expr) {
    if (expr.cond) {
        expr.cond->accept(*this);
    }
    if (expr.body) {
        expr.body->accept(*this);
    }
}


namespace {
// C type of an expression as the *printed* C will see it (LP64), which can
// differ from llvm2c's model: `243 ^ (long long)x` is modelled with the IR
// constant's unsigned type but is signed in C (decimal literals are signed).
struct CType { bool known = false; bool uns = false; int bits = 0; };

int bitsOfName(std::string n) {
    for (const char* p : {"unsigned ", "signed "})
        if (n.rfind(p, 0) == 0) n = n.substr(strlen(p));
    if (n == "char") return 8;
    if (n == "short") return 16;
    if (n == "int") return 32;
    if (n == "long" || n == "long long") return 64;
    if (n == "__int128") return 128;
    return 0;
}

CType fromType(Type* t) {
    auto IT = llvm::dyn_cast_or_null<IntegerType>(t);
    if (!IT) return {};
    int b = bitsOfName(IT->toString());
    if (!b) return {};
    return {true, IT->unsignedType, b};
}

CType promote(CType c) {
    if (c.known && c.bits < 32) return {true, false, 32};
    return c;
}

CType usualConversions(CType a, CType b) {
    if (!a.known || !b.known) return {};
    a = promote(a); b = promote(b);
    if (a.uns == b.uns) return {true, a.uns, std::max(a.bits, b.bits)};
    CType u = a.uns ? a : b, s = a.uns ? b : a;
    return u.bits >= s.bits ? u : s;
}

bool isIntLiteral(const Expr* e) {
    if (e->getKind() != Expr::EK_Value) return false;
    const std::string& n = static_cast<const Value*>(e)->valueName;
    size_t i = (!n.empty() && n[0] == '-') ? 1 : 0;
    return i < n.size() && std::all_of(n.begin() + i, n.end(), ::isdigit);
}

CType cType(Expr* e) {
    if (isIntLiteral(e)) {
        errno = 0;
        long long v = strtoll(static_cast<Value*>(e)->valueName.c_str(), nullptr, 10);
        bool fitsInt = errno == 0 && v >= INT_MIN && v <= INT_MAX;
        return {true, false, fitsInt ? 32 : 64};
    }
    if (llvm::isa<CastExpr>(e)) return fromType(e->getType());
    if (llvm::isa<AddExpr>(e) || llvm::isa<SubExpr>(e) || llvm::isa<MulExpr>(e) ||
        llvm::isa<DivExpr>(e) || llvm::isa<RemExpr>(e) || llvm::isa<AndExpr>(e) ||
        llvm::isa<OrExpr>(e) || llvm::isa<XorExpr>(e)) {
        auto* b = static_cast<BinaryExpr*>(e);
        return usualConversions(cType(b->left), cType(b->right));
    }
    if (llvm::isa<ShlExpr>(e) || llvm::isa<AshrExpr>(e) || llvm::isa<LshrExpr>(e))
        return promote(cType(static_cast<BinaryExpr*>(e)->left));
    return fromType(e->getType());
}
} // namespace

// castIfNeeded trusts the modelled operand types.  For operations whose
// result depends on signedness (ordered compare, div, rem), also check what
// C will actually compute and cast the non-literal operands if it disagrees.
void SignCastsVisitor::enforceCSignedness(BinaryExpr& expr, bool isUnsigned) {
    CType c = usualConversions(cType(expr.left), cType(expr.right));
    if (!c.known || c.uns == isUnsigned)
        return;
    for (Expr** side : {&expr.left, &expr.right}) {
        CType s = cType(*side);
        auto IT = llvm::dyn_cast_or_null<IntegerType>((*side)->getType());
        if (!IT || isIntLiteral(*side) || (s.known && promote(s).uns == isUnsigned))
            continue;
        Type* target = IT->unsignedType == isUnsigned ? IT : toggleSignedness(IT);
        auto cast = std::make_unique<CastExpr>(*side, target);
        *side = cast.get();
        block->addOwnership(std::move(cast));
    }
}

// C's `>>` is arithmetic or logical depending on the type of its left
// operand, so make that type (unpromoted, of the IR width) match the opcode.
void SignCastsVisitor::enforceShiftSignedness(BinaryExpr& expr, bool isUnsigned) {
    expr.left = castIfNeeded(expr.left, isUnsigned);
    auto IT = llvm::dyn_cast_or_null<IntegerType>(expr.left->getType());
    if (!IT)
        return;
    CType s = cType(expr.left);
    int irBits = bitsOfName(IT->toString());
    if (!irBits || (s.known && s.uns == isUnsigned && s.bits == irBits))
        return;
    Type* target = IT->unsignedType == isUnsigned ? IT : toggleSignedness(IT);
    auto cast = std::make_unique<CastExpr>(expr.left, target);
    expr.left = cast.get();
    block->addOwnership(std::move(cast));
}

Expr* SignCastsVisitor::castIfNeeded(Expr* expr, bool isUnsigned) {
    Expr* result = expr;
    auto IT = llvm::dyn_cast_or_null<IntegerType>(expr->getType());

    if (IT && IT->unsignedType != isUnsigned) {
        Type* newType = toggleSignedness(IT);

        auto cast = std::make_unique<CastExpr>(expr, newType);
        result = cast.get();
        block->addOwnership(std::move(cast));
    }

    return result;
}

void SignCastsVisitor::visit(CmpExpr& expr) {
    // cast are not necessary in these cases
    // if (expr.comparsion == "==" || expr.comparsion == "!=")
    //     return;
    // WHY ?
    expr.left->accept(*this);
    expr.right->accept(*this);
    
    expr.left = castIfNeeded(expr.left, expr.isUnsigned);
    expr.right = castIfNeeded(expr.right, expr.isUnsigned);
    if (expr.comparsion != "==" && expr.comparsion != "!=")
        enforceCSignedness(expr, expr.isUnsigned);
}

void SignCastsVisitor::visit(AggregateElement& expr) {
    expr.expr->accept(*this);
}

void SignCastsVisitor::visit(ArrayElement& ae) {
    ae.expr->accept(*this);
    ae.element->accept(*this);
}

void SignCastsVisitor::visit(ExtractValueExpr& expr) {
    for (auto& index : expr.indices) {
        index->accept(*this);
    }
}

void SignCastsVisitor::visit(SwitchExpr& expr) {
    expr.cmp->accept(*this);
}

void SignCastsVisitor::visit(AsmExpr& /*expr*/) {
}

void SignCastsVisitor::visit(CallExpr& expr) {
    if (expr.funcValue) {
        expr.funcValue->accept(*this);
    }

    for (auto it = expr.params.begin(); it != expr.params.end(); ++it) {
        (*it)->accept(*this);
    }
}

void SignCastsVisitor::visit(PointerShift& expr) {
    expr.pointer->accept(*this);
    expr.move->accept(*this);
}

void SignCastsVisitor::visit(GepExpr& expr) {
    for (auto& index : expr.indices) {
        index->accept(*this);
    }
}

void SignCastsVisitor::visit(SelectExpr& expr) {
    expr.left->accept(*this);
    expr.right->accept(*this);
    expr.comp->accept(*this);
}

void SignCastsVisitor::visit(RefExpr& expr) {
    expr.expr->accept(*this);
}

void SignCastsVisitor::visit(DerefExpr& expr) {
    expr.expr->accept(*this);
}

void SignCastsVisitor::visit(RetExpr& expr) {
    if (expr.expr) {
        expr.expr->accept(*this);
    }
}

void SignCastsVisitor::visit(CastExpr& expr) {
    expr.expr->accept(*this);
}

void SignCastsVisitor::visit(AddExpr& expr) {
    expr.left->accept(*this);
    expr.right->accept(*this);

    expr.left = castIfNeeded(expr.left, expr.isUnsigned);
    expr.right = castIfNeeded(expr.right, expr.isUnsigned);
}

void SignCastsVisitor::visit(SubExpr& expr) {
    expr.left->accept(*this);
    expr.right->accept(*this);

    expr.left = castIfNeeded(expr.left, expr.isUnsigned);
    expr.right = castIfNeeded(expr.right, expr.isUnsigned);
}

void SignCastsVisitor::visit(AssignExpr& expr) {
    expr.left->accept(*this);
    expr.right->accept(*this);
}

void SignCastsVisitor::visit(MulExpr& expr) {
    expr.left->accept(*this);
    expr.right->accept(*this);

    expr.left = castIfNeeded(expr.left, expr.isUnsigned);
    expr.right = castIfNeeded(expr.right, expr.isUnsigned);
}

void SignCastsVisitor::visit(DivExpr& expr) {
    expr.left->accept(*this);
    expr.right->accept(*this);

    expr.left = castIfNeeded(expr.left, expr.isUnsigned);
    expr.right = castIfNeeded(expr.right, expr.isUnsigned);
    enforceCSignedness(expr, expr.isUnsigned);
}

void SignCastsVisitor::visit(RemExpr& expr) {
    expr.left->accept(*this);
    expr.right->accept(*this);

    expr.left = castIfNeeded(expr.left, expr.isUnsigned);
    expr.right = castIfNeeded(expr.right, expr.isUnsigned);
    enforceCSignedness(expr, expr.isUnsigned);
}

void SignCastsVisitor::visit(AndExpr& expr) {
    expr.left->accept(*this);
    expr.right->accept(*this);
}

void SignCastsVisitor::visit(OrExpr& expr) {
    expr.left->accept(*this);
    expr.right->accept(*this);
}

void SignCastsVisitor::visit(XorExpr& expr) {
    expr.left->accept(*this);
    expr.right->accept(*this);
}

void SignCastsVisitor::visit(ShlExpr& expr) {
    expr.left->accept(*this);
    expr.right->accept(*this);

    expr.left = castIfNeeded(expr.left, expr.isUnsigned);
    expr.right = castIfNeeded(expr.right, expr.isUnsigned);
}

void SignCastsVisitor::visit(AshrExpr& expr) {
    expr.left->accept(*this);
    expr.right->accept(*this);
    enforceShiftSignedness(expr, false);
}

void SignCastsVisitor::visit(LshrExpr& expr) {
    expr.left->accept(*this);
    expr.right->accept(*this);
    enforceShiftSignedness(expr, true);
}

void SignCastsVisitor::visit(MinusExpr& expr) {
    expr.expr->accept(*this);
}

void SignCastsVisitor::visit(LogicalNot& expr) {
    expr.expr->accept(*this);
}

void SignCastsVisitor::visit(LogicalAnd& expr) {
    expr.lhs->accept(*this);
    expr.rhs->accept(*this);
}

void SignCastsVisitor::visit(LogicalOr& expr) {
    expr.lhs->accept(*this);
    expr.rhs->accept(*this);
}

void SignCastsVisitor::visit(ArrowExpr& expr) {
    expr.expr->accept(*this);
}

IntegerType* SignCastsVisitor::toggleSignedness(IntegerType* ty) {
    return program.typeHandler.toggleSignedness(ty);
}
