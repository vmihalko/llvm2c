#include "ProgramParser.h"
#include "passes.h"

#include <llvm/IR/Constants.h>
#include <llvm/IRReader/IRReader.h>
#include <iostream>

Program ProgramParser::parse(const std::string& file) {
    Program result;
    llvm::LLVMContext context;

    auto error = llvm::SMDiagnostic();
    auto module = llvm::parseIRFile(file, error, context);
    if (!module) {
        throw std::invalid_argument("Error loading module - invalid input file:\n" + file + "\n");
    }

    const auto* mod = module.get();
    parseGlobalVars(mod, result);
    parseStructs(mod, result);

    determineIncludes(mod, result);
    findMetadataNames(mod, result);
    findDeclaredFunctions(mod, result);
    createFunctions(mod, result);
    nameFunctions(mod, result);
    createFunctionParameters(mod, result);
    createBlocks(mod, result);
    identifyInlinableBlocks(mod, result);
    createAllocas(mod, result);
    parseMetadataTypes(mod, result);
    createExpressions(mod, result);
    addPhis(mod, result);
    parseBreaks(mod, result);

    // transformations of resulting expressions
    deleteUnusedVariables(mod, result);
    fixMainParameters(mod, result);
    addSignCasts(mod, result);
    deleteRedundantCasts(mod, result);

    refDeref(mod, result);

    return result;
}
