#pragma once

#include "../core/Program.h"
#include "CWriter.h"

#include <iostream>

/**
 * @brief Writer converts programs to C code
 */
class Writer
{
private:
    CWriter wr;

    bool hideStreams;

    void includes(const Program& program);
    void structDeclarations(const Program& program);
    void structDefinitions(const Program& program);
    void globalVars(const Program& program);
    void anonymousStructDeclarations(const Program& program);
    void globalVarDefinitions(const Program& program);
    void functionDeclarations(const Program& program);
    void anonymousStructDefinitions(const Program& program);
    void functionDefinitions(const Program& program);
    void typedefs(const Program& program);
    void structDefinition(const Struct* strct);
    bool isFunctionPrinted(const Func* func) const;
    void functionHead(const Func* func);


public:
    Writer(std::ostream& stream, bool hideStreams) : wr(CWriter(stream)), hideStreams(hideStreams) {}
    void writeProgram(const Program& program);
};
