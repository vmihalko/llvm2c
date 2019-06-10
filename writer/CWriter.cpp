#include "CWriter.h"


void CWriter::include(StrRef header) {
    out << "#include <" << header << ">" << std::endl;
}

void CWriter::declareStruct(StrRef name) {
    out << "struct " << name << ";" << std::endl;
}

void CWriter::comment(StrRef comment) {
    out << "// " << comment << std::endl;
}

void CWriter::startStruct(StrRef name) {
    out << "struct " << name << " {" << std::endl;
}

void CWriter::endStruct() {
    out << "};" << std::endl;
}

void CWriter::indent(size_t tabs) {
    for (auto t = 0; t < tabs; ++t) {
        out << "    ";
    }
}

void CWriter::structItem(StrRef ty, StrRef name) {
    out << ty << " " << name << ";" << std::endl;
}

void CWriter::defineType(StrRef ty, StrRef alias, StrRef end) {
    out << "typedef " << ty << " " << alias << end << ";" << std::endl;
}

void CWriter::startFunction(StrRef ret, StrRef name) {
    out << ret << " " << name << "(";
}
void CWriter::endFunctionDecl() {
    out << ");" << std::endl;
}

void CWriter::functionParam(StrRef type, StrRef name) {
    out << type << " " << name;
}

void CWriter::nextFunctionParam() {
    out << ", ";
}
