#include "Writer.h"
#include "../parser/cfunc.h"

#include <unordered_set>

void Writer::writeProgram(const Program& program) {
    includes(program);
    wr.line("");
    structDeclarations(program);
    wr.line("");
    typedefs(program);
    wr.line("");
    structDefinitions(program);
    wr.line("");
    globalVars(program);
    wr.line("");
    anonymousStructDeclarations(program);
    wr.line("");
    globalVarDefinitions(program);
    wr.line("");
    functionDeclarations(program);
    wr.line("");
    anonymousStructDefinitions(program);
    wr.line("");
    functionDefinitions(program);
    wr.line("");
}

void Writer::includes(const Program& program) {
    if (!useIncludes)
        return;

    wr.comment("includes");
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
}

void Writer::structDeclarations(const Program& program) {
    wr.comment("struct declarations");
    const auto& structs = program.structs;

    for (const auto& strct : structs) {
        wr.declareStruct(strct->name);
    }
}

void Writer::structDefinition(const Struct* strct) {
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
    wr.comment("struct definitions");
    std::unordered_set<std::string> printed;

    for (const auto& strct : program.structs) {
        for (const auto& item : strct->items) {

            auto type = item.first.get();
            if (auto AT = dynamic_cast<ArrayType*>(type)) {
                if (AT->isStructArray) {
                    if (printed.insert(AT->structName).second)
                        structDefinition(program.getStruct(AT->structName));
                }
            }

            if (auto PT = dynamic_cast<PointerType*>(type)) {
                if (PT->isStructPointer && PT->isArrayPointer) {
                    if (printed.insert(PT->structName).second)
                        structDefinition(program.getStruct(PT->structName));
                }
            }

            if (auto ST = dynamic_cast<StructType*>(type)) {
                if (printed.insert(ST->name).second)
                    structDefinition(program.getStruct(ST->name));
            }
        }

        if (printed.insert(strct->name).second)
            structDefinition(strct.get());
    }
}

void Writer::typedefs(const Program& program) {
    wr.comment("type definitions");
    const auto& defs = program.typeHandler.sortedTypeDefs;

    for (const auto& def : defs) {
        wr.defineType(def->type, def->name, def->typeEnd);
    }
}

void Writer::anonymousStructDeclarations(const Program& program) {
    wr.comment("anonymous struct declarations");
    const auto& structs = program.unnamedStructs;

    for (const auto& elem : structs) {
        wr.declareStruct(elem.second->name);
    }
}

void Writer::globalVars(const Program& program) {
    wr.comment("global variable declarations");
    for (const auto& gvar : program.globalVars) {
        if (useIncludes && (gvar->valueName == "stdin" || gvar->valueName == "stdout" || gvar->valueName == "stderr")) {
            continue;
        }

        wr.declareVar(gvar->getType()->toString(), gvar->getType()->surroundName(gvar->valueName));
    }
}

void Writer::globalVarDefinitions(const Program& program) {
    wr.comment("global variable definitions");
    for (const auto& gvar : program.globalVars) {
        if (useIncludes && (gvar->valueName == "stdin" || gvar->valueName == "stdout" || gvar->valueName == "stderr")) {
            continue;
        }

        wr.raw(gvar->getType()->toString());
        wr.raw(" ");
        wr.raw(gvar->getType()->surroundName(gvar->valueName));
        wr.raw(" = ");
        wr.raw(gvar->value);
        wr.line(";");
    }
}

void Writer::functionHead(const Func* func) {
    auto PT = dynamic_cast<PointerType*>(func->returnType.get());
    bool arrayPtr = (PT && PT->isArrayPointer);
    if (arrayPtr) {
        wr.startArrayFunction(func->returnType->toString(), PT->levels, func->name);
    } else {
        wr.startFunction(func->returnType->toString(), func->name);
    }

    auto last = func->parameters.cend() - 1;
    wr.startFunctionParams();
    for (auto it = func->parameters.cbegin(); it != func->parameters.cend(); ++it) {
        const auto& param = *it;
        wr.raw(param->getType()->toString());
        wr.raw(" ");
        param->accept(ew);

        if (it != last)
            wr.nextFunctionParam();
    }

    if (func->isVarArg) {
        if (!func->parameters.empty()) {
            wr.nextFunctionParam();
        }
        wr.functionVarArgs();
    }
    wr.endFunctionParams();

    if (arrayPtr) {
        wr.raw(")");
        wr.raw(PT->sizes);
    }
}

void Writer::functionDeclarations(const Program& program) {
    wr.comment("function declarations");
    for (const auto& decl : program.declarations) {
        auto& func = decl.second;
        if (!isFunctionPrinted(func.get())) {
            continue;
        }

        functionHead(func.get());

        wr.endFunctionDecl();
    }
}

void Writer::anonymousStructDefinitions(const Program& program) {
    wr.comment("anonymous struct definitions");
    std::unordered_set<std::string> printed;

    for (const auto& pair : program.unnamedStructs) {
        const auto& strct = pair.second;
        for (const auto& item : strct->items) {

            auto type = item.first.get();
            if (auto AT = dynamic_cast<ArrayType*>(type)) {
                if (AT->isStructArray) {
                    if (printed.insert(AT->structName).second)
                        structDefinition(program.getStruct(AT->structName));
                }
            }

            if (auto PT = dynamic_cast<PointerType*>(type)) {
                if (PT->isStructPointer && PT->isArrayPointer) {
                    if (printed.insert(PT->structName).second)
                        structDefinition(program.getStruct(PT->structName));
                }
            }

            if (auto ST = dynamic_cast<StructType*>(type)) {
                if (printed.insert(ST->name).second)
                    structDefinition(program.getStruct(ST->name));
            }
        }

        if (printed.insert(strct->name).second)
            structDefinition(strct.get());
    }
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

void Writer::writeBlock(const Block* block, bool first) {
    std::unordered_set<Value*> initialized;

    if (!first) {
        wr.startBlock(block->blockName);
    }

    for (const auto& expr : block->expressions) {
        // TODO: introduce StackAlloc expression to use instead of this
        if (auto V = dynamic_cast<Value*>(expr)) {
            wr.indent(1);
            if (initialized.insert(V).second) {
                wr.declareVar(expr->getType()->toString(), V->getType()->surroundName(V->valueName));
            }
            continue;
        }

        wr.indent(1);
        expr->accept(ew);
        wr.line(";");
    }
}

void Writer::functionDefinitions(const Program& program) {
    for (const auto& pair : program.functions) {
        const auto* func = pair.second.get();
        if (!isFunctionPrinted(func)) {
            continue;
        }

        functionHead(func);
        wr.startFunctionBody();

        // start with phi variables
        // TODO: prepend phi variables of function to the first block instead of this hack
        for (const auto& var : func->phiVariables) {
            wr.indent(1);
            wr.declareVar(var->getType()->toString(), var->getType()->surroundName(var->valueName));
        }

        bool first = true;
        for (const auto& blockEntry : func->blockMap) {
            const auto* block = blockEntry.second.get();
            writeBlock(block, first);
            first = false;
        }

        wr.endFunctionBody();
    }
}
