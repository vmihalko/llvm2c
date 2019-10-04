#include "ProgramParser.h"
#include "passes.h"

#include <llvm/IR/Constants.h>
#include <llvm/IRReader/IRReader.h>
#include <iostream>

#define RUN_PASS(pass) \
    do {\
        puts("    Starting pass: " #pass );\
        pass(mod, result); \
    } while (0);


Program ProgramParser::parse(const std::string& file) {
    Program result;
    llvm::LLVMContext context;

    auto error = llvm::SMDiagnostic();
    auto module = llvm::parseIRFile(file, error, context);
    if (!module) {
        throw std::invalid_argument("Error loading module - invalid input file:\n" + file + "\n");
    }

    const auto* mod = module.get();

    std::cerr << "Parser start" << std::endl;
    RUN_PASS(determineIncludes);
    RUN_PASS(parseStructDeclarations);
    RUN_PASS(parseStructItems);
    RUN_PASS(findDeclaredFunctions);
    RUN_PASS(computeGlobalVarsOrder);
    RUN_PASS(createFunctions);
    RUN_PASS(findMetadataFunctionNames);
    RUN_PASS(nameFunctions);

    RUN_PASS(createConstants);
    RUN_PASS(initializeGlobalVars);

    RUN_PASS(createFunctionParameters);
    RUN_PASS(createBlocks);
    RUN_PASS(createAllocas);

    RUN_PASS(parseMetadataTypes);
    RUN_PASS(prepareBitcastUnion);
    RUN_PASS(createExpressions);

    RUN_PASS(parseBreaks);

    RUN_PASS(refDeref);
    RUN_PASS(memcpyToAssignment);
    RUN_PASS(arrowify);
    RUN_PASS(deleteUnusedVariables);
    RUN_PASS(fixMainParameters);
    RUN_PASS(addSignCasts);
    RUN_PASS(deleteRedundantCasts);
    RUN_PASS(extractVars);
    RUN_PASS(identifyInlinableBlocks);
    RUN_PASS(inlineBlocks);
    std::cerr << "Parser end" << std::endl;

    return result;
}
