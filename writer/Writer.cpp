#include <algorithm>
#include "Writer.h"
#include "../parser/cfunc.h"

#include <unordered_set>

#define SECTION_START(name, cond)\
    bool __sectionEnabled = ((cond));\
    if (__sectionEnabled) {\
        wr.comment(name);\
    }

#define SECTION_END\
    if (__sectionEnabled) {\
        wr.line("");\
    }

void Writer::writeProgram(const Program& program) {
    includes(program);
    structDeclarations(program);
    anonymousStructDeclarations(program);
    typedefs(program);
    structDefinitions(program);
    unionDefinitions(program);
    functionDeclarations(program);
    writeIntrinsicDefinitions(program);
    globalVarDefinitions(program);
    functionDefinitions(program);
}

void Writer::includes(const Program& program) {
    if (program.hasCMath && !useIncludes) {
        wr.line("int __isnanf( float x ) { return x != x; }");
        wr.line("int __isnan( double x ) { return x != x; }");
    }

    // Array aggregate copies are rendered as memmove by ExprWriter.  The call
    // is synthesised by the writer, so it is not in the module and
    // functionDeclarations() will not declare it.  Without -add-includes there
    // is no <string.h> either, which would leave memmove implicitly declared
    // (invalid C99+, hard error on GCC 14+/C23).  Declare it explicitly.
    // size_t is `unsigned long` under the artifact's pinned LP64 data model.
    if (program.hasString && !useIncludes) {
        wr.line("void *memmove(void *, const void *, unsigned long);");
    }

    if (!useIncludes)
        return;

    SECTION_START("includes", (program.hasVarArg || program.hasStdLib || program.hasString || program.hasStdio || program.hasPthread || program.hasCMath));

    if (program.hasVarArg)
        wr.include("stdarg.h");

    if (program.hasStdLib)
        wr.include("stdlib.h");

    if (program.hasString)
        wr.include("string.h");

    if (program.hasStdio)
        wr.include("stdio.h");

    if (program.hasPthread)
        wr.include("pthread.h");

    if (program.hasCMath)
        wr.include("math.h");

    SECTION_END;
}

void Writer::structDeclarations(const Program& program) {
    const auto& structs = program.structs;
    SECTION_START("struct declarations", structs.size() + program.unions.size() > 0);

    for (const auto& strct : structs) {
        wr.declareStruct(strct->name);
    }

    for (const auto& unn : program.unions) {
        wr.declareUnion(unn->name);
    }

    SECTION_END;
}

void Writer::structDefinition(const Program& program, const StructType* strct, std::unordered_set<const StructType*>& printed) {
    for (const auto& item : strct->items) {
        const auto type = item.first;
        if (auto AT = llvm::dyn_cast_or_null<ArrayType>(type)) {
            if (AT->isStructArray) {
                structDefinition(program, program.getStruct(AT->structName), printed);
            }
        }

        if (auto PT = llvm::dyn_cast_or_null<PointerType>(type)) {
            if (PT->isStructPointer && PT->isArrayPointer) {
                structDefinition(program, program.getStruct(PT->structName), printed);
            }
        }

        if (auto ST = llvm::dyn_cast_or_null<StructType>(type)) {
            structDefinition(program, program.getStruct(ST->name), printed);
        }
    }

    auto it = printed.find(strct);
    if (it != printed.end()) {
        return;
    }

    printed.insert(it, strct);

    wr.startStruct(strct->name);

    for (const auto& pair : strct->items) {
        const auto& ty = pair.first;
        const auto& name = pair.second;
        wr.indent(1);
        wr.structItem(ty->toString(), ty->surroundName(name));
    }

    wr.endStruct();
}

void Writer::structDefinitions(const Program& program) {
    SECTION_START("struct definitions", program.structs.size() + program.unnamedStructs.size() > 0);
    std::unordered_set<const StructType*> printed;

    for (const auto& strct : program.structs) {
        structDefinition(program, strct.get(), printed);
    }

    for (const auto& pair : program.unnamedStructs) {
        const auto& strct = pair.second;
        structDefinition(program, strct.get(), printed);
    }

    SECTION_END;
}

void Writer::unionDefinition(const Program& /*program*/, const UnionType* strct) {
    wr.startUnion(strct->name);

    for (const auto& pair : strct->items) {
        const auto& ty = pair.first;
        const auto& name = pair.second;
        wr.indent(1);
        wr.structItem(ty->toString(), ty->surroundName(name));
    }

    wr.endStruct();
}


void Writer::unionDefinitions(const Program& program) {
    SECTION_START("union definitions", !program.unions.empty());

    for (const auto& unn : program.unions) {
        unionDefinition(program, unn.get());
    }

    SECTION_END;
}


void Writer::typedefs(const Program& program) {
    SECTION_START("type definitions", !program.typeHandler.sortedTypeDefs.empty());
    const auto& defs = program.typeHandler.sortedTypeDefs;

    for (const auto& def : defs) {
        wr.defineType(def->type, def->name, def->typeEnd);
    }

    SECTION_END;
}

void Writer::anonymousStructDeclarations(const Program& program) {
    SECTION_START("anonymous struct declarations", !program.unnamedStructs.empty());
    const auto& structs = program.unnamedStructs;

    for (const auto& elem : structs) {
        wr.declareStruct(elem.second->name);
    }

    SECTION_END;
}

void Writer::globalVars(const Program& program) {
    SECTION_START("global variable declarations", !program.globalVars.empty());
    for (const auto& gvar : program.globalVars) {
        if (useIncludes && (gvar->valueName == "stdin" || gvar->valueName == "stdout" || gvar->valueName == "stderr")) {
            continue;
        }

        wr.declareVar(gvar->getType()->toString(), gvar->getType()->surroundName(gvar->valueName));
    }

    SECTION_END;
}

void Writer::globalVarDefinitions(const Program& program) {
    SECTION_START("global variable definitions", !program.globalVars.empty());
    for (const auto& gvar : program.globalVars) {
        if (useIncludes && (gvar->valueName == "stdin" || gvar->valueName == "stdout" || gvar->valueName == "stderr")) {
            continue;
        }

        if (gvar->isStatic) {
            wr.raw("static ");
        }
        if (gvar->isExtern && !gvar->value) {  // Only extern if no initializer
            wr.raw("extern ");
        }
        wr.raw(gvar->getType()->toString());
        wr.raw(" ");
        if (gvar->isConst) {
            wr.raw("const ");
        }
        wr.raw(gvar->getType()->surroundName(gvar->valueName));

        if (gvar->value) {
            wr.raw(" = ");
            gvar->value->accept(ew);
        }

        wr.line(";");
    }

    SECTION_END;
}

void Writer::functionHead(const Func* func, bool isdecl) {
    if (func->isDeclaration)
        wr.raw("extern ");

    const auto PT = llvm::dyn_cast_or_null<PointerType>(func->returnType);
    bool arrayPtr = (PT && PT->isArrayPointer);
    if (arrayPtr) {
        wr.startArrayFunction(func->returnType->toString(), PT->levels, func->name);
    } else {
        wr.startFunction(func->returnType->toString(), func->name);
    }

    //auto last = func->parameters;
    auto last = func->parameters.cend();
    if (func->parameters.size())
        last--;
    wr.startFunctionParams();
    for (auto it = func->parameters.cbegin(); it != func->parameters.cend(); ++it) {
        const auto& param = *it;

        const auto ppt = llvm::dyn_cast_or_null<PointerType>(param->getType());
        bool paramArrayPtr = (ppt && ppt->isArrayPointer);
        if (paramArrayPtr) {
            wr.startArrayFunction(param->getType()->toString(), ppt->levels, "");
            param->accept(ew);
            wr.raw(")");
            wr.raw(ppt->arraySizes());
        } else {
            wr.raw(param->getType()->toString());
            wr.raw(" ");
            param->accept(ew);
        }

        if (it != last)
            wr.nextFunctionParam();
    }

    if (func->isVarArg) {
        if (func->parameters.empty()) {
            // if we are printing only the declaration of a vararg function,
            // do not print '...'. This is the default in C.
            if (!isdecl) {
                wr.functionVarArgs();
            }
        } else {
            wr.nextFunctionParam();
            wr.functionVarArgs();
        }
    } else if (func->parameters.empty()) {
        wr.functionNoArgs();
    }
    wr.endFunctionParams();

    if (arrayPtr) {
        wr.raw(")");
        wr.raw(PT->arraySizes());
    }
}

void Writer::functionDeclarations(const Program& program) {
    std::vector<Func*> declarations;

    for (const auto& decl : program.functions) {
        auto& func = decl.second;
        if (!isFunctionPrinted(func.get())) {
            continue;
        }

        // Skip intrinsics that will have definitions generated
        std::string funcName = func->name;
        std::string originalName = funcName;
        std::replace(originalName.begin(), originalName.end(), '_', '.');
        if (program.intrinsicsNeedingDefinitions.find(originalName) != 
            program.intrinsicsNeedingDefinitions.end()) {
            continue;  // Skip - will be defined later
        }

        declarations.push_back(func.get());
    }

    std::sort(declarations.begin(), declarations.end(), [](const Func* a, const Func* b){ return a->name <= b->name;});

    SECTION_START("function declarations", !declarations.empty());
    for (const auto* func : declarations) {
        functionHead(func, /* isdecl = */ true);
        wr.endFunctionDecl();
    }

    SECTION_END;
}

void Writer::writeIntrinsicDefinitions(const Program& program) {
    if (program.intrinsicsNeedingDefinitions.empty()) {
        return;
    }
    
    SECTION_START("LLVM intrinsic definitions", true);
    
    for (const auto& intrinsicName : program.intrinsicsNeedingDefinitions) {
        std::string cName = intrinsicName;
        std::replace(cName.begin(), cName.end(), '.', '_');
        
        // Determine types from intrinsic name
        bool is32bit = cName.find("i32") != std::string::npos;
        bool isUnsigned = cName.find("uadd") != std::string::npos || 
                         cName.find("usub") != std::string::npos ||
                         cName.find("umul") != std::string::npos;
        
        // Use signed types for signed intrinsics (sadd, ssub, smul)
        std::string argType;
        if (is32bit) {
            argType = isUnsigned ? "unsigned int" : "int";
        } else {
            argType = isUnsigned ? "unsigned long long" : "long long";
        }
        std::string structName;
        std::string structVar1;
        std::string structVar2;
        
        // Find the actual struct type by searching for calls to this intrinsic in the program
        // The intrinsic name in CallExpr may already have dots replaced with underscores,
        // so we need to check both the original and transformed names
        StructType* structType = nullptr;
        std::string intrinsicNameWithDots = intrinsicName;  // Original name with dots
        std::string intrinsicNameWithUnderscores = cName;   // Transformed name with underscores
        
        for (const auto& funcPair : program.functions) {
            const Func* func = funcPair.second.get();
            for (const auto& blockPair : func->blockMap) {
                const Block* block = blockPair.second.get();
                for (Expr* expr : block->expressions) {
                    // Check if this is a direct CallExpr
                    if (auto* call = llvm::dyn_cast_or_null<CallExpr>(expr)) {
                        std::string callName = call->funcName;
                        std::string callNameNormalized = callName;
                        std::replace(callNameNormalized.begin(), callNameNormalized.end(), '.', '_');
                        
                        if (callName == intrinsicNameWithDots || 
                            callName == intrinsicNameWithUnderscores ||
                            callNameNormalized == intrinsicNameWithUnderscores) {
                            Type* returnType = call->getType();
                            if (auto* st = llvm::dyn_cast_or_null<StructType>(returnType)) {
                                structType = st;
                                break;
                            }
                        }
                    }
                    // Also check AssignExpr->right, as CallExpr may be wrapped in an assignment
                    if (auto* assign = llvm::dyn_cast_or_null<AssignExpr>(expr)) {
                        if (auto* call = llvm::dyn_cast_or_null<CallExpr>(assign->right)) {
                            std::string callName = call->funcName;
                            std::string callNameNormalized = callName;
                            std::replace(callNameNormalized.begin(), callNameNormalized.end(), '.', '_');
                            
                            if (callName == intrinsicNameWithDots || 
                                callName == intrinsicNameWithUnderscores ||
                                callNameNormalized == intrinsicNameWithUnderscores) {
                                Type* returnType = call->getType();
                                if (auto* st = llvm::dyn_cast_or_null<StructType>(returnType)) {
                                    structType = st;
                                    break;
                                }
                            }
                        }
                    }
                    // Also check ExtractValueExpr, as the intrinsic call may be wrapped in extractvalue
                    if (auto* extract = llvm::dyn_cast_or_null<ExtractValueExpr>(expr)) {
                        if (!extract->indices.empty()) {
                            // Get the first index which should contain the CallExpr
                            Expr* firstExpr = extract->indices[0].get();
                            // Unwrap AggregateElement to get to the CallExpr
                            Expr* unwrappedExpr = firstExpr;
                            while (auto* agg = llvm::dyn_cast_or_null<AggregateElement>(unwrappedExpr)) {
                                unwrappedExpr = agg->expr;
                            }
                            if (auto* call = llvm::dyn_cast_or_null<CallExpr>(unwrappedExpr)) {
                                std::string callName = call->funcName;
                                std::string callNameNormalized = callName;
                                std::replace(callNameNormalized.begin(), callNameNormalized.end(), '.', '_');
                                
                                if (callName == intrinsicNameWithDots || 
                                    callName == intrinsicNameWithUnderscores ||
                                    callNameNormalized == intrinsicNameWithUnderscores) {
                                    Type* returnType = call->getType();
                                    if (auto* st = llvm::dyn_cast_or_null<StructType>(returnType)) {
                                        structType = st;
                                        break;
                                    }
                                }
                            }
                            // Also check if the first AggregateElement has a struct type
                            // The AggregateElement's expr should be the CallExpr, and its type should be the struct type
                            if (auto* agg = llvm::dyn_cast_or_null<AggregateElement>(firstExpr)) {
                                // The type of AggregateElement's expr is the struct type
                                Type* aggExprType = agg->expr->getType();
                                if (auto* st = llvm::dyn_cast_or_null<StructType>(aggExprType)) {
                                    // This is accessing a struct member, so the struct type is the type of the expression
                                    structType = st;
                                    break;
                                }
                                // Also check if the expr itself is a CallExpr with the intrinsic name
                                if (auto* call = llvm::dyn_cast_or_null<CallExpr>(agg->expr)) {
                                    std::string callName = call->funcName;
                                    std::string callNameNormalized = callName;
                                    std::replace(callNameNormalized.begin(), callNameNormalized.end(), '.', '_');
                                    
                                    if (callName == intrinsicNameWithDots || 
                                        callName == intrinsicNameWithUnderscores ||
                                        callNameNormalized == intrinsicNameWithUnderscores) {
                                        Type* returnType = call->getType();
                                        if (auto* st = llvm::dyn_cast_or_null<StructType>(returnType)) {
                                            structType = st;
                                            break;
                                        }
                                    }
                                }
                            }
                        }
                    }
                    // Also check RetExpr, as the return might contain ExtractValueExpr
                    if (auto* ret = llvm::dyn_cast_or_null<RetExpr>(expr)) {
                        if (ret->expr) {
                            if (auto* extract = llvm::dyn_cast_or_null<ExtractValueExpr>(ret->expr)) {
                                if (!extract->indices.empty()) {
                                    Expr* firstExpr = extract->indices[0].get();
                                    if (auto* agg = llvm::dyn_cast_or_null<AggregateElement>(firstExpr)) {
                                        Type* aggExprType = agg->expr->getType();
                                        if (auto* st = llvm::dyn_cast_or_null<StructType>(aggExprType)) {
                                            // Check if the expr is a CallExpr with the intrinsic name
                                            if (auto* call = llvm::dyn_cast_or_null<CallExpr>(agg->expr)) {
                                                std::string callName = call->funcName;
                                                std::string callNameNormalized = callName;
                                                std::replace(callNameNormalized.begin(), callNameNormalized.end(), '.', '_');
                                                
                                                if (callName == intrinsicNameWithDots || 
                                                    callName == intrinsicNameWithUnderscores ||
                                                    callNameNormalized == intrinsicNameWithUnderscores) {
                                                    structType = st;
                                                    break;
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
                if (structType) break;
            }
            if (structType) break;
        }
        
        if (!structType) {
            // If no call found, try to find the struct type from the program's structs
            // Look for anonymous structs that match the expected pattern (2 members, one is the result type, one is _Bool)
            for (const auto& st : program.structs) {
                if (st->items.size() == 2) {
                    // Check if this looks like an overflow struct:
                    // - Has exactly 2 members
                    // - Second member is _Bool (the overflow flag)
                    Type* secondType = st->items[1].first;
                    if (auto* boolType = llvm::dyn_cast_or_null<BoolType>(secondType)) {
                        // This looks like an overflow struct - prefer anonymous structs
                        if (st->name.find("anonymous") != std::string::npos || 
                            st->name.find("structVar") != std::string::npos) {
                            structType = st.get();
                            break;
                        }
                        // If no anonymous struct found yet, remember this one
                        if (!structType) {
                            structType = st.get();
                        }
                    }
                }
            }
        }
        
        if (!structType) {
            // If still no struct found, skip generating this definition
            // The intrinsic might not be used in this program, or the search failed
            continue;
        }
        
        if (structType->items.size() < 2) {
            // Invalid struct for overflow intrinsic - skip
            continue;
        }
        
        structName = "struct " + structType->name;
        // First member is the result value, second is the overflow flag
        structVar1 = structType->items[0].second;
        structVar2 = structType->items[1].second;
        
        // Determine operation and overflow check
        std::string maxVal = is32bit ? "2147483647" : "9223372036854775807LL";
        std::string minVal = is32bit ? "(-2147483648)" : "(-9223372036854775807LL - 1)";

        std::string op = "+";
        std::string overflowCheck;
        const bool isSub = cName.find("sub") != std::string::npos;
        const bool isMul = cName.find("mul") != std::string::npos;

        // Precise signed predicates only behind --precise-signed-overflow;
        // default emits the legacy heuristic checks (bit-compatible with
        // upstream/stock llvm2c output).
        const bool precise = preciseSignedOverflow && !isUnsigned;

        if (isSub) {
            op = "-";
            if (!precise) {
                overflowCheck = "(a < b)";
            } else {
                overflowCheck = "(b <= 0 ? a > " + maxVal + " + b : a < " + minVal + " + b)";
            }
        } else if (isMul) {
            op = "*";
            if (!precise) {
                overflowCheck = "(a != 0 && (sum / a) != b)";
            } else {
                // UB-free signed mul check; handles INT_MIN * -1 via b < MAX / a branch
                overflowCheck = "(a > 0 ? (b > 0 ? a > " + maxVal + " / b : (b < 0 ? b < " +
                                  minVal + " / a : 0)) : (a < 0 ? (b > 0 ? a < " + minVal +
                                  " / b : (b < 0 ? (a != 0 && b < " + maxVal + " / a) : 0)) : 0))";
            }
        } else {
            if (!precise) {
                overflowCheck = "(sum < a || sum < b)";
            } else {
                overflowCheck = "(b > 0 ? a > " + maxVal + " - b : a < " + minVal + " - b)";
            }
        }
        
        // Generate function definition
        wr.raw(structName);
        wr.raw(" ");
        wr.raw(cName);
        wr.raw("(");
        wr.raw(argType);
        wr.raw(" a, ");
        wr.raw(argType);
        wr.raw(" b) {");
        wr.line("");
        
        wr.indent(1);
        wr.raw(structName);
        wr.raw(" result;");
        wr.line("");
        
        wr.indent(1);
        // Use direct arithmetic - for signed types, overflow is UB which verifiers
        // can assume doesn't happen. For unsigned types, wrap-around is well-defined.
        wr.raw(argType);
        wr.raw(" sum = a ");
        wr.raw(op);
        wr.raw(" b;");
        wr.line("");
        
        wr.indent(1);
        wr.raw("result.");
        wr.raw(structVar1);
        wr.raw(" = sum;  // Wrapped result");
        wr.line("");
        
        wr.indent(1);
        wr.raw("result.");
        wr.raw(structVar2);
        wr.raw(" = ");
        wr.raw(overflowCheck);
        wr.raw(";  // Overflow flag");
        wr.line("");
        
        wr.indent(1);
        wr.raw("return result;");
        wr.line("");
        
        wr.raw("}");
        wr.line("");
    }
    
    SECTION_END;
}

bool Writer::isFunctionPrinted(const Func* func) const {
    if (isCFunc(func->name) || func->name == "va_start" || func->name == "va_end" || func->name == "va_copy" || isCMath(func->name)) {
        return false;
    }

    if (useIncludes) {
        if (isStdLibFunc(func->name) || isStringFunc(func->name) || isStdioFunc(func->name) || isPthreadFunc(func->name)) {
            return false;
        }
    } else {
        if ((func->name == "memcpy" || func->name == "memset" || func->name == "memmove") && func->parameters.size() > 3) {
            return false;
        }
    }

    return true;
}

void Writer::writeBlock(const Block* block) {
    wr.indent(1);
    wr.startBlock(block->blockName);

    for (const auto& expr : block->expressions) {
        wr.indent(1);
        expr->accept(ew);
        if (!llvm::isa<IfExpr>(expr) && !llvm::isa<SwitchExpr>(expr) && !llvm::isa<ExprList>(expr)) {
            wr.line(";");
        }
    }
}

void Writer::functionDefinitions(const Program& program) {
    std::vector<const Func*> functions;

    for (const auto& pair : program.functions) {
        const auto* func = pair.second.get();
        if (func->isDeclaration) {
            continue;
        }

        if (!isFunctionPrinted(func)) {
            continue;
        }

        functions.push_back(func);
    }

    std::sort(functions.begin(), functions.end(), [](const Func* a, const Func* b){ return a->name <= b->name;});

    for (const auto* func : functions) {

        functionHead(func);
        wr.startFunctionBody();

        // start with variables
        for (const auto& var : func->variables) {
            if( func->parameters.end() != std::find_if(func->parameters.begin(), func->parameters.end(), [&var](Value *v){return var->valueName == v->valueName;})) continue;
            wr.indent(1);
            wr.declareVar(var->getType()->toString(), var->getType()->surroundName(var->valueName));
        }

        assert(func->entry);
        writeBlock(func->entry);

        for (const auto& blockEntry : func->blockMap) {
            const auto* block = blockEntry.second.get();

            if (block == func->entry)
                continue;

            if (!block->doInline) {
                writeBlock(block);
            }
        }

        wr.endFunctionBody();
        wr.line("");
    }
}
