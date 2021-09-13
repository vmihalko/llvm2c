#pragma once

#include "../core/Program.h"


class ProgramParser
{
private:

public:
    ProgramParser() = default;
    Program parse(const std::string& from, bool bitcastUnions);
    virtual ~ProgramParser() = default;
};
