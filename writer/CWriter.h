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
    void endFunctionDecl();
    void functionParam(StrRef ty, StrRef name);
    void nextFunctionParam();
};
