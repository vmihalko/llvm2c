#pragma once

#include "../core/Program.h"


class ProgramParser
{
private:

public:
    ProgramParser() = default;
    Program parse(const std::string& from);
    virtual ~ProgramParser() = default;
};
