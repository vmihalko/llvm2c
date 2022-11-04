#include "ProgramParser.h"
#include "passes.h"

#include <llvm/IR/Constants.h>
#include <llvm/IRReader/IRReader.h>
#include <iostream>

#define RUN_PASS(pass) \
    do {\
        pass(mod, result); \
    } while (0);


Program ProgramParser::parse(const std::string& file, bool bitcastUnions) {
    Program result;
    llvm::LLVMContext context;

    auto error = llvm::SMDiagnostic();
    auto module = llvm::parseIRFile(file, error, context);
    if (!module) {
        throw std::invalid_argument("Error loading module - invalid input file:\n" + file + "\n");
    }

    const auto* mod = module.get();

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
    RUN_PASS(findMetadataVariableNames);

    // This pass is buggy. It can do signed type from
    // unsigned type when the original type is unsigned...
    // Do not use it until we fix that.
    //RUN_PASS(parseMetadataTypes);

    if (bitcastUnions) {
        RUN_PASS(prepareBitcastUnion);
    }
    //RUN_PASS(createExpressions);
    createExpressions(mod, result, bitcastUnions);

    RUN_PASS(parseBreaks);

    RUN_PASS(refDeref);
    //RUN_PASS(memcpyToAssignment);
    RUN_PASS(arrowify);
    RUN_PASS(deleteUnusedVariables);
    RUN_PASS(fixMainParameters);
    RUN_PASS(addSignCasts);
    // This pass is not working well. It does not take
    // into account the signdness of types and can
    // remove casts that are needed, e.g.:
    // It can do (int)var from (int)(short)var
    // where var is (unsigned short).
    //RUN_PASS(deleteRedundantCasts);
    RUN_PASS(extractVars);
    RUN_PASS(identifyInlinableBlocks);
    RUN_PASS(inlineBlocks);

    return result;
}
