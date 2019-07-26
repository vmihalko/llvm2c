#include "Expr.h"
#include "UnaryExpr.h"

#include "llvm/Support/raw_ostream.h"

Struct::Struct(const std::string& name)
    : ExprBase(EK_Struct), name(name) {
    setType(std::make_unique<StructType>(this->name));
}

void Struct::addItem(std::unique_ptr<Type> type, const std::string& name) {
    items.push_back(std::make_pair(std::move(type), name));
}

void Struct::accept(ExprVisitor& visitor) {
    visitor.visit(*this);
}

bool Struct::classof(const Expr* expr) {
    return expr->getKind() == EK_Struct;
}

StructElement::StructElement(Struct* strct, Expr* expr, unsigned element)
    : ExprBase(EK_StructElement),
      strct(strct),
      expr(expr),
      element(element) {
    setType(strct->items[element].first->clone());
}

void StructElement::accept(ExprVisitor& visitor) {
    visitor.visit(*this);
}

bool StructElement::classof(const Expr* expr) {
    return expr->getKind() == EK_StructElement;
}

ArrayElement::ArrayElement(Expr* expr, Expr* elem)
    : ExprBase(EK_ArrayElement),
      expr(expr),
      element(elem) {
    ArrayType* AT = static_cast<ArrayType*>(expr->getType());
    setType(AT->type->clone());
}

ArrayElement::ArrayElement(Expr* expr, Expr* elem, std::unique_ptr<Type> type)
    : ExprBase(EK_ArrayElement),
      expr(expr),
      element(elem) {
    setType(std::move(type));
}

void ArrayElement::accept(ExprVisitor& visitor) {
    visitor.visit(*this);
}

bool ArrayElement::classof(const Expr* expr) {
    return expr->getKind() == EK_ArrayElement;
}

ExtractValueExpr::ExtractValueExpr(std::vector<std::unique_ptr<Expr>>& indices)
    : ExprBase(EK_ExtractValueExpr) {
    for (auto& idx : indices) {
        this->indices.push_back(std::move(idx));
    }

    setType(this->indices[this->indices.size() - 1]->getType()->clone());
}

void ExtractValueExpr::accept(ExprVisitor& visitor) {
    visitor.visit(*this);
}

bool ExtractValueExpr::classof(const Expr* expr) {
    return expr->getKind() == EK_ExtractValueExpr;
}

Value::Value(const std::string& valueName, std::unique_ptr<Type> type)
    : ExprBase(EK_Value) {
    setType(std::move(type));
    this->valueName = valueName;
}

Value::Value(const std::string& valueName, std::unique_ptr<Type> type, ExprKind kind)
    : ExprBase(kind) {
    setType(std::move(type));
    this->valueName = valueName;
}

void Value::accept(ExprVisitor& visitor) {
    visitor.visit(*this);
}

bool Value::isZero() const {
    return valueName == "0";
}

bool Value::isSimple() const {
    return true;
}

bool Value::classof(const Expr* expr) {
    return expr->getKind() == EK_Value || expr->getKind() == EK_GlobalValue;
}

GlobalValue::GlobalValue(const std::string& varName, Expr* value, std::unique_ptr<Type> type)
    : Value(varName, std::move(type), EK_GlobalValue),
      value(value) { }

void GlobalValue::accept(ExprVisitor& visitor) {
    visitor.visit(*this);
}

bool GlobalValue::classof(const Expr* expr) {
    return expr->getKind() == EK_GlobalValue;
}

bool GlobalValue::isSimple() const {
    return true;
}

IfExpr::IfExpr(Expr* cmp, Expr* trueList, Expr* falseList)
    : ExprBase(EK_IfExpr),
      cmp(cmp),
      trueList(trueList),
      falseList(falseList) {}

void IfExpr::accept(ExprVisitor& visitor) {
    visitor.visit(*this);
}

bool IfExpr::classof(const Expr* expr) {
    return expr->getKind() == EK_IfExpr;
}

SwitchExpr::SwitchExpr(Expr* cmp, Expr* def, std::map<int, Expr*> cases)
    : ExprBase(EK_SwitchExpr),
      cmp(cmp),
      def(def),
      cases(cases) {}

void SwitchExpr::accept(ExprVisitor& visitor) {
    visitor.visit(*this);
}

bool SwitchExpr::classof(const Expr* expr) {
    return expr->getKind() == EK_SwitchExpr;
}

AsmExpr::AsmExpr(const std::string& inst, const std::vector<std::pair<std::string, Expr*>>& output, const std::vector<std::pair<std::string, Expr*>>& input, const std::string& clobbers)
    : ExprBase(EK_AsmExpr),
      inst(inst),
      output(output),
      input(input),
      clobbers(clobbers) {}

void AsmExpr::addOutputExpr(Expr* expr, unsigned pos) {
    for (unsigned i = pos; i < output.size(); i++) {
        if (!output[i].second) {
            output[i].second = expr;
            break;
        }
    }
}

void AsmExpr::accept(ExprVisitor& visitor) {
    visitor.visit(*this);
}

bool AsmExpr::classof(const Expr* expr) {
    return expr->getKind() == EK_AsmExpr;
}

CallExpr::CallExpr(Expr* funcValue, const std::string &funcName, std::vector<Expr*> params, std::unique_ptr<Type> type)
    : ExprBase(EK_CallExpr),
      funcName(funcName),
      params(params),
      funcValue(funcValue) {
    setType(std::move(type));
}

void CallExpr::accept(ExprVisitor& visitor) {
    visitor.visit(*this);
}

bool CallExpr::classof(const Expr* expr) {
    return expr->getKind() == EK_CallExpr;
}

bool CallExpr::isSimple() const {
    return true;
}

PointerShift::PointerShift(std::unique_ptr<Type> ptrType, Expr* pointer, Expr* move)
    : ExprBase(EK_PointerShift),
      ptrType(std::move(ptrType)),
      pointer(pointer),
      move(move) {
    if (auto PT = llvm::dyn_cast_or_null<PointerType>(this->ptrType.get())) {
        setType(PT->type->clone());
    }
}

void PointerShift::accept(ExprVisitor& visitor) {
    visitor.visit(*this);
}

bool PointerShift::classof(const Expr* expr) {
    return expr->getKind() == EK_PointerShift;
}

GepExpr::GepExpr(std::vector<Expr*>&& indices)
    : ExprBase(EK_GepExpr), indices(std::move(indices)) {

    setType(this->indices[this->indices.size() - 1]->getType()->clone());
}

void GepExpr::accept(ExprVisitor& visitor) {
    visitor.visit(*this);
}

bool GepExpr::classof(const Expr* expr) {
    return expr->getKind() == EK_GepExpr;
}

SelectExpr::SelectExpr(Expr* comp, Expr* l, Expr* r) :
    ExprBase(EK_SelectExpr),
    left(l),
    right(r),
    comp(comp) {
    setType(l->getType()->clone());
}

void SelectExpr::accept(ExprVisitor& visitor) {
    visitor.visit(*this);
}

bool SelectExpr::classof(const Expr* expr) {
    return expr->getKind() == EK_SelectExpr;
}

StackAlloc::StackAlloc(Value* var):
    ExprBase(EK_StackAlloc),
    value(var) {
    setType(var->getType()->clone());
}

void StackAlloc::accept(ExprVisitor& visitor) {
    visitor.visit(*this);
}

bool StackAlloc::classof(const Expr* expr) {
    return expr->getKind() == EK_StackAlloc;
}

AggregateInitializer::AggregateInitializer(std::vector<Expr*> values): ExprBase(EK_AggregateInitializer), values(values) { }

bool AggregateInitializer::classof(const Expr* expr) {
    return expr->getKind() == EK_AggregateInitializer;
}

void AggregateInitializer::accept(ExprVisitor& visitor) {
    visitor.visit(*this);
}

ArrowExpr::ArrowExpr(Struct* strct, Expr* expr, unsigned element) : ExprBase(EK_ArrowExpr), strct(strct), expr(expr), element(element) {
    setType(strct->items[element].first->clone());
}

void ArrowExpr::accept(ExprVisitor& visitor) {
    visitor.visit(*this);
}

bool ArrowExpr::classof(const Expr* expr) {
    return expr->getKind() == EK_ArrowExpr;
}

LogicalAnd::LogicalAnd(Expr* lhs, Expr* rhs): ExprBase(EK_LogicalAnd), lhs(lhs), rhs(rhs) {
    setType(std::make_unique<IntegerType>("int", false));
}

void LogicalAnd::accept(ExprVisitor& visitor) {
    visitor.visit(*this);
}

bool LogicalAnd::classof(const Expr* expr) {
    return expr->getKind() == EK_LogicalAnd;
}

LogicalOr::LogicalOr(Expr* lhs, Expr* rhs): ExprBase(EK_LogicalOr), lhs(lhs), rhs(rhs) {
    setType(std::make_unique<IntegerType>("int", false));
}

void LogicalOr::accept(ExprVisitor& visitor) {
    visitor.visit(*this);
}

bool LogicalOr::classof(const Expr* expr) {
    return expr->getKind() == EK_LogicalOr;
}

GotoExpr::GotoExpr(Block* target) : ExprBase(EK_GotoExpr), target(target) { }

void GotoExpr::accept(ExprVisitor& visitor) {
    visitor.visit(*this);
}

bool GotoExpr::classof(const Expr* expr) {
    return expr->getKind() == EK_GotoExpr;
}

ExprList::ExprList(std::vector<Expr*>&& expressions) : ExprBase(EK_ExprList), expressions(std::move(expressions)) {}

void ExprList::accept(ExprVisitor& visitor) {
    visitor.visit(*this);
}

bool ExprList::classof(const Expr* expr) {
    return expr->getKind() == EK_ExprList;
}
