#pragma once

#include <iostream>

class CWriter
{
private:
    using StreamRef = std::ostream&;
    using StrRef = const std::string&;

    StreamRef out;


public:
    CWriter(StreamRef stream) : out(stream) {}
    void include(StrRef header);
    void comment(StrRef comment);
    void declareStruct(StrRef name);
    void startStruct(StrRef name);
    void endStruct();
    void indent(size_t tabs);
    void structItem(StrRef ty, StrRef name);
    void defineType(StrRef ty, StrRef alias, StrRef end);
    void startFunction(StrRef ret, StrRef name);
    void startArrayFunction(StrRef ret, size_t levels, StrRef name);
    void endFunctionDecl();
    void functionParam(StrRef ty, StrRef name);
    void nextFunctionParam();

    // TODO: get rid of raw
    void raw(StrRef line);

    // TODO: get rid of line
    void line(StrRef line);

    void functionVarArgs();
    void startFunctionParams();
    void endFunctionParams();
    void startFunctionBody();
    void endFunctionBody();
    void declareVar(StrRef ty, StrRef name);
    void startBlock(StrRef label);
};
