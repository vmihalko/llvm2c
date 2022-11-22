#include "ExprWriter.h"

#include "../core/Block.h"

void ExprWriter::indent() {
    for (size_t i = 0; i < indentCount; ++i) {
        ss << "    ";
    }
}

ExprWriter::ExprWriter(std::ostream& os, bool noFuncCasts, bool forceBlockLabels): ss(os), noFuncCasts(noFuncCasts), forceBlockLabels(forceBlockLabels) { }

/* void ExprWriter::visit(Struct& expr) { */
/*     ss << "struct "; */
/*     ss << expr.name << " {" << std::endl; */

/*     for (const auto& item : expr.items) { */
/*         std::string faPointer; */

/*         ss << "    " << item.first->toString(); */

/*         if (auto PT = llvm::dyn_cast_or_null<PointerType>(item.first.get())) { */
/*             if (PT->isArrayPointer) { */
/*                 faPointer = " ("; */
/*                 for (unsigned i = 0; i < PT->levels; i++) { */
/*                     faPointer += "*"; */
/*                 } */
/*                 faPointer += item.second + ")" + PT->sizes; */
/*             } */
/*         } */

/*         if (faPointer.empty()) { */
/*             ss << " "; */

/*             if (auto AT = llvm::dyn_cast_or_null<ArrayType>(item.first.get())) { */
/*                 if (AT->isPointerArray && AT->pointer->isArrayPointer) { */
/*                     ss << "("; */
/*                     for (unsigned i = 0; i < AT->pointer->levels; i++) { */
/*                       ss << "*"; */
/*                     } */
/*                     ss << item.second << AT->sizeToString() << ")" << AT->pointer->sizes; */
/*                 } else { */
/*                     ss << item.second << AT->sizeToString(); */
/*                 } */
/*             } else { */
/*                 ss << item.second; */
/*             } */
/*         } else { */
/*             ss << faPointer; */
/*         } */

/*         ss << ";\n"; */
/*     } */

/*     ss << "};"; */
/* } */

void ExprWriter::visit(AggregateElement& elem) {
    parensIfNotSimple(elem.expr);

    if (llvm::dyn_cast_or_null<PointerType>(elem.expr->getType())) {
        ss << "->";
    } else {
        ss << ".";
    }

    auto ty = llvm::dyn_cast<AggregateType>(elem.expr->getType());
    ss << ty->items[elem.element].second;
}

void ExprWriter::visit(ArrayElement& ae) {
    parensIfNotSimple(ae.expr);
    ss << "[";
    ae.element->accept(*this);
    ss << "]";
}

void ExprWriter::visit(ExtractValueExpr& expr) {
    const auto& last = expr.indices.back();
    last->accept(*this);
}

void ExprWriter::visit(Value& expr) {
    ss << expr.valueName;
}

void ExprWriter::visit(GlobalValue& expr) {
    ss << expr.valueName;
}

void ExprWriter::visit(IfExpr& expr) {
    ss << "if (";
    expr.cmp->accept(*this);
    ss << ") {" << std::endl;
    indentCount++;
    indent();
    expr.trueList->accept(*this);
    indentCount--;
    indent();
    ss << "} else {" << std::endl;
    indentCount++;
    indent();
    expr.falseList->accept(*this);
    indentCount--;
    indent();
    ss << "}" << std::endl;
}

void ExprWriter::visit(GotoExpr& expr) {
    ss << "goto " << expr.target->blockName;
}

void ExprWriter::visit(SwitchExpr& expr) {
    ss << "switch (";
    expr.cmp->accept(*this);
    ss << ") {" << std::endl;

    for (const auto& lb_block : expr.cases) {
        const auto& label = lb_block.first;
        const auto& block = lb_block.second;

        indentCount++;
        indent();
        ss << "case " << label << ":" << std::endl;
        indentCount++;
        indent();
        block->accept(*this);
        indentCount--;
        indentCount--;
    }

    if (expr.def) {
        indentCount++;
        indent();
        ss << "default:" << std::endl;
        indentCount++;
        indent();
        expr.def->accept(*this);
        indentCount--;
        indentCount--;
    }

    ss << "}" << std::endl;
}

void ExprWriter::visit(AsmExpr& expr) {
    ss << "__asm__(\"" << expr.inst << "\"" << std::endl;
    indentCount++;
    indent();
    ss << ": ";
    if (!expr.output.empty()) {
        bool first = true;
        for (const auto& out : expr.output) {
            if (!out.second) {
                break;
            }
            if (!first) {
                ss << ", ";
            }
            first = false;

            ss << out.first << " (";
            out.second->accept(*this);
            ss << ")";
        }
    }

    ss << std::endl;
    indent();
    ss << ": ";

    if (!expr.input.empty()) {
        bool first = true;
        for (const auto& in : expr.input) {
            if (!first) {
                ss << ", ";
            }
            first = false;

            ss << in.first << " (";
            in.second->accept(*this);
            ss << ")";
        }
    }

    ss << std::endl;
    indent();
    ss << ": ";

    if (!expr.clobbers.empty()) {
        ss << expr.clobbers;
    }

    ss << std::endl;

    indentCount--;
    indent();
    ss << ")";

}

void ExprWriter::visit(CallExpr& expr) {
    bool isVaFunc = (expr.funcName == "va_start" || expr.funcName == "va_end");

    if (expr.funcValue) {
        auto call = expr.funcValue;

        if (noFuncCasts) {
            // strip all the casts
            while (auto CAST = llvm::dyn_cast_or_null<CastExpr>(call)) {
                call = CAST->expr;
            }
        }

        parensIfNotSimple(call);
    } else {
        ss << expr.funcName;
    }

    ss << "(";
    if (isVaFunc) {
        ss << "(void*)(";
    }

    bool first = true;
    for (auto param : expr.params) {
        if (first) {
            param->accept(*this);
            if (isVaFunc) {
                ss << ")";
            }

        } else {
            ss << ", ";
            param->accept(*this);
        }

        first = false;
    }

    ss << ")";
}

void ExprWriter::visit(PointerShift& expr) {
    if (expr.move->isZero()) {
        expr.pointer->accept(*this);
        return;
    }

    ss << "*(((" << expr.ptrType->toString();

    const auto PT = static_cast<PointerType*>(expr.ptrType);

    if (PT->isArrayPointer) {
        ss << "(";
        for (unsigned i = 0; i < PT->levels; i++) {
            ss << "*";
        }
        ss << ")" << PT->sizes;
    }

    ss << ")(";
    expr.pointer->accept(*this);
    ss << ")) + ";
    parensIfNotSimple(expr.move);
    ss << ")";
}

void ExprWriter::visit(GepExpr& expr) {
    expr.indices.back()->accept(*this);
}

void ExprWriter::visit(SelectExpr& expr) {
    parensIfNotSimple(expr.comp);
    ss << " ? ";
    parensIfNotSimple(expr.left);
    ss << " : ";
    parensIfNotSimple(expr.right);
}

void ExprWriter::visit(RefExpr& ref) {
    ss << "&";
    parensIfNotSimple(ref.expr);
}

void ExprWriter::visit(DerefExpr& deref) {
    ss << "*";
    parensIfNotSimple(deref.expr);
}

void ExprWriter::visit(RetExpr& ret) {
    ss << "return";
    if (ret.expr) {
        ss << " ";
        ret.expr->accept(*this);
    }
}

void ExprWriter::writeCastType(Type *Ty) {
    ss << "(" << Ty->toString();
    if (auto PT = llvm::dyn_cast_or_null<const PointerType>(Ty)) {
        if (PT->isArrayPointer) {
            ss << " (";
            for (unsigned i = 0; i < PT->levels; i++) {
                ss << "*";
            }
            ss << ")" + PT->sizes;
        }
    }
    ss << ")";
}

void ExprWriter::visit(CastExpr& cast) {
    writeCastType(cast.getType());
    parensIfNotSimple(cast.expr);
}

void ExprWriter::visit(AddExpr& expr) {
    parensIfNotSimple(expr.left);
    ss << " + ";
    parensIfNotSimple(expr.right);
}

void ExprWriter::visit(SubExpr& expr) {
    parensIfNotSimple(expr.left);
    ss << " - ";
    parensIfNotSimple(expr.right);
}

void ExprWriter::visit(AssignExpr& expr) {
    parensIfNotSimple(expr.left);
    ss << " = ";
    parensIfNotSimple(expr.right);
}

void ExprWriter::visit(MulExpr& expr) {
    parensIfNotSimple(expr.left);
    ss << " * ";
    parensIfNotSimple(expr.right);
}

void ExprWriter::visit(DivExpr& expr) {
    parensIfNotSimple(expr.left);
    ss << " / ";
    parensIfNotSimple(expr.right);
}

void ExprWriter::visit(RemExpr& expr) {
    parensIfNotSimple(expr.left);
    ss << " % ";
    parensIfNotSimple(expr.right);
}

void ExprWriter::visit(AndExpr& expr) {
    parensIfNotSimple(expr.left);

    if (llvm::isa<BoolType>(expr.left->getType()) &&
        llvm::isa<BoolType>(expr.right->getType())) {
        ss << " && ";
    } else {
        ss << " & ";
    }

    parensIfNotSimple(expr.right);
}

void ExprWriter::visit(OrExpr& expr) {
    parensIfNotSimple(expr.left);
    ss << " | ";
    parensIfNotSimple(expr.right);
}

void ExprWriter::visit(XorExpr& expr) {
    parensIfNotSimple(expr.left);
    ss << " ^ ";
    parensIfNotSimple(expr.right);
}

void ExprWriter::visit(CmpExpr& expr) {
    parensIfNotSimple(expr.left);
    ss << " " << expr.comparsion << " ";
    parensIfNotSimple(expr.right);
}

void ExprWriter::visit(AshrExpr& expr) {
    parensIfNotSimple(expr.left);
    ss << " >> ";
    parensIfNotSimple(expr.right);
}

void ExprWriter::visit(LshrExpr& expr) {
    auto IT = static_cast<IntegerType*>(expr.left->getType());
    if (!IT->unsignedType) {
        ss << "(unsigned " << IT->toString() << ")(";
    } else {
        ss << "(";
    }
    expr.left->accept(*this);
    ss << ") >> (";
    expr.right->accept(*this);
    ss << ")";
}

void ExprWriter::visit(ShlExpr& expr) {
    parensIfNotSimple(expr.left);
    ss << " << ";
    parensIfNotSimple(expr.right);
}

void ExprWriter::visit(StackAlloc& expr) {
    ss << expr.getType()->toString();
    ss << " ";
    ss << expr.getType()->surroundName(expr.value->valueName);
}

void ExprWriter::visit(ArrowExpr& expr) {
    parensIfNotSimple(expr.expr);
    ss << "->";
    auto pty = llvm::dyn_cast<PointerType>(expr.expr->getType());
    auto ty = llvm::dyn_cast<AggregateType>(pty->type);
    ss << ty->items[expr.element].second;
}

void ExprWriter::visit(ExprList& exprList) {
    bool first = true;
    for (const auto& expr : exprList.expressions) {
        if (!first)
            indent();
        first = false;
        expr->accept(*this);
        if (!llvm::isa<IfExpr>(expr) && !llvm::isa<SwitchExpr>(expr) && !llvm::isa<ExprList>(expr)) {
            ss << ";" << std::endl;
        }
    }
}

void ExprWriter::parensIfNotSimple(Expr* expr) {
    if (!expr->isSimple()) {
        ss << "(";
    }

    expr->accept(*this);

    if (!expr->isSimple()) {
        ss << ")";
    }
}

void ExprWriter::visit(AggregateInitializer& expr) {
    ss << "{";
    for (auto& val : expr.values) {
        val->accept(*this);
        ss << ",";
    }
    ss << "}";
}

void ExprWriter::visit(LogicalAnd& expr) {
    parensIfNotSimple(expr.lhs);
    ss << " && ";
    parensIfNotSimple(expr.rhs);
}

void ExprWriter::visit(LogicalOr& expr) {
    parensIfNotSimple(expr.lhs);
    ss << " || ";
    parensIfNotSimple(expr.rhs);
}

void ExprWriter::visit(MinusExpr& expr) {
    ss << "-";
    parensIfNotSimple(expr.expr);
}

void ExprWriter::visit(LogicalNot& expr) {
    ss << "!";
    parensIfNotSimple(expr.expr);
}

void ExprWriter::visit(DoWhile& expr) {
    ss << "do {" << std::endl;
    indentCount++;
    indent();
    expr.body->accept(*this);
    indentCount--;
    indent();
    ss << "} while (";
    expr.cond->accept(*this);
    ss << ")";
}

void ExprWriter::visit(Do& expr) {
    ss << "do {" << std::endl;
    expr.body->accept(*this);
}

void ExprWriter::visit(While& expr) {
    ss << "do {" << std::endl;
    expr.body->accept(*this);
    ss << "} while (";
    expr.cond->accept(*this);
    ss << ");" << std::endl;
}
