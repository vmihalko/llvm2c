#pragma once

#include "../core/Program.h"
#include "CWriter.h"
#include "ExprWriter.h"

#include <iostream>
#include <unordered_set>

/**
 * @brief Writer converts programs to C code
 */
class Writer
{
private:
    CWriter wr;
    ExprWriter ew;

    bool useIncludes;
    bool noFuncCasts;
    bool forceBlockLabels;

    void includes(const Program& program);
    void unionDeclarations(const Program& program);
    void structDeclarations(const Program& program);
    void structDefinitions(const Program& program);
    void globalVars(const Program& program);
    void anonymousStructDeclarations(const Program& program);
    void globalVarDefinitions(const Program& program);
    void functionDeclarations(const Program& program);
    void anonymousStructDefinitions(const Program& program);
    void functionDefinitions(const Program& program);
    void typedefs(const Program& program);
    void structDefinition(const Program& program, const StructType* strct, std::unordered_set<const StructType*>& printed);
    bool isFunctionPrinted(const Func* func) const;
    void functionHead(const Func* func, bool isdecl=false);
    void writeBlock(const Block* block);
    void unionDefinition(const Program& program, const UnionType* unn);
    void unionDefinitions(const Program& program);


public:
    Writer(std::ostream& stream, bool useIncludes, bool noFuncCasts, bool forceBlockLabels) : wr(CWriter(stream)), ew(ExprWriter(stream, noFuncCasts, forceBlockLabels)), useIncludes(useIncludes), noFuncCasts(true), forceBlockLabels(forceBlockLabels) {}
    void writeProgram(const Program& program);
};
