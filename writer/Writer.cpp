#include "Writer.h"

#include <unordered_set>

void Writer::writeProgram(const Program& program) {
    includes(program);
    structDeclarations(program);
    typedefs(program);
    structDefinitions(program);
    globalVars(program);
    anonymousStructDeclarations(program);
    globalVarDefinitions(program);
    functionDeclarations(program);
    anonymousStructDefinitions(program);
    functionDefinitions(program);
}

void Writer::includes(const Program& program) {
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
    wr.comment("structure declarations");
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
        wr.structItem(ty->toString(), name);
    }

    wr.endStruct();
}

void Writer::structDefinitions(const Program& program) {
    wr.comment("structure definitions");
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
    wr.comment("anonymous structure declarations");
    const auto& structs = program.unnamedStructs;

    for (const auto& elem : structs) {
        wr.declareStruct(elem.second->name);
    }
}

void Writer::globalVars(const Program& program) {
    wr.comment("global variable declarations");
    for (const auto& gvar : program.globalVars) {
        if (hideStreams && (gvar->valueName == "stdin" || gvar->valueName == "stdout" || gvar->valueName == "stderr")) {
            continue;
        }
        wr.comment(gvar->valueName);
    }
}

void Writer::globalVarDefinitions(const Program& program) {
    wr.comment("global variable definitions");
    for (const auto& gvar : program.globalVars) {
        if (hideStreams && (gvar->valueName == "stdin" || gvar->valueName == "stdout" || gvar->valueName == "stderr")) {
            continue;
        }
        wr.comment(gvar->toString());
    }
}

void Writer::functionDeclarations(const Program& program) {
    wr.comment("function declarations");
    for (const auto& decl : program.declarations) {
        auto& func = decl.second;
        wr.startFunction(func->returnType->toString(), func->name);

        auto last = func->parameters.cend() - 1;
        for (auto it = func->parameters.cbegin(); it != func->parameters.cend(); ++it) {
            const auto& param = *it;
            wr.functionParam(param->getType()->toString(), param->toString());
            if (it != last)
                wr.nextFunctionParam();
        }

        wr.endFunctionDecl();
    }
}

void Writer::anonymousStructDefinitions(const Program& program) {}
void Writer::functionDefinitions(const Program& program) {}
