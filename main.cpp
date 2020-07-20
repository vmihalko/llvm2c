#include "core/Program.h"
#include "parser/ProgramParser.h"
#include "writer/Writer.h"

#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/CommandLine.h"

#include <iostream>
#include <fstream>
#include <string>

using namespace llvm;

int main(int argc, char** argv) {
    cl::OptionCategory options("llvm2c options");
    cl::opt<std::string> Output("o", cl::desc("Output filename"), cl::value_desc("filename"), cl::cat(options));
    cl::opt<std::string> Input(cl::Positional, cl::Required, cl::desc("<input>"), cl::cat(options));
    cl::opt<bool> Print("p", cl::desc("Print translated program"), cl::cat(options));
    cl::opt<bool> Includes("add-includes", cl::desc("Uses includes instead of declarations. For experimental purposes."), cl::cat(options));
    cl::opt<bool> Casts("no-function-call-casts", cl::desc("Removes casts around function calls. For experimental purposes."), cl::cat(options));
    cl::opt<bool> BlockLabels("force-block-labels", cl::desc("Forces printing of block labels of inlined blocks"), cl::cat(options));

    cl::HideUnrelatedOptions(options);
    cl::ParseCommandLineOptions(argc, argv);

    if (Output.empty() && !Print) {
        std::cout << "Output method not specified!\n";
        return 1;
    }

    try {
        ProgramParser parser;
        auto program = parser.parse(Input);

        if (Print) {
            Writer wr{ std::cout, Includes, Casts, BlockLabels };
            wr.writeProgram(program);
        }

        if (!Output.empty()) {
            std::ofstream file;
            file.open(Output);
            if (!file.is_open()) {
                throw std::invalid_argument("Output file cannot be opened!");
            }
            Writer wr{ file, Includes, Casts, BlockLabels };
            wr.writeProgram(program);
        }

    } catch (std::invalid_argument& e) {
        std::cerr << e.what();
        return 1;
    }

    return 0;
}
