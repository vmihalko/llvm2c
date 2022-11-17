#include "ProgramParser.h"
#include "passes.h"

#include <llvm/IR/Constants.h>
#include <llvm/IRReader/IRReader.h>
#include <iostream>


#include <llvm/Transforms/Utils/SimplifyCFGOptions.h>
#include <functional>
#include "llvm/Analysis/GlobalsModRef.h"
#include "llvm/Analysis/Passes.h"
#include "llvm/Analysis/LoopPass.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/Verifier.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/DynamicLibrary.h"
#include "llvm/Support/PluginLoader.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Transforms/IPO.h"
#include "llvm/Transforms/IPO/FunctionAttrs.h"
#include "llvm/Transforms/Scalar.h"
#include "llvm/Transforms/Scalar/GVN.h"

#include "llvm/Transforms/IPO/PassManagerBuilder.h"
#include "llvm-c/Transforms/PassManagerBuilder.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/Analysis/BasicAliasAnalysis.h"
#include "llvm/Analysis/CFLAndersAliasAnalysis.h"
#include "llvm/Analysis/CFLSteensAliasAnalysis.h"
#include "llvm/Analysis/GlobalsModRef.h"
#include "llvm/Analysis/InlineCost.h"
#include "llvm/Analysis/Passes.h"
#include "llvm/Analysis/ScopedNoAliasAA.h"
#include "llvm/Analysis/TargetLibraryInfo.h"
#include "llvm/Analysis/TypeBasedAliasAnalysis.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/Verifier.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/ManagedStatic.h"
#include "llvm/Transforms/AggressiveInstCombine/AggressiveInstCombine.h"
#include "llvm/Transforms/IPO.h"
#include "llvm/Transforms/IPO/Attributor.h"
#include "llvm/Transforms/IPO/ForceFunctionAttrs.h"
#include "llvm/Transforms/IPO/FunctionAttrs.h"
#include "llvm/Transforms/IPO/InferFunctionAttrs.h"
#include "llvm/Transforms/InstCombine/InstCombine.h"
#include "llvm/Transforms/Instrumentation.h"
#include "llvm/Transforms/Scalar.h"
#include "llvm/Transforms/Scalar/GVN.h"
#include "llvm/Transforms/Scalar/InstSimplifyPass.h"
#include "llvm/Transforms/Scalar/LICM.h"
#include "llvm/Transforms/Scalar/LoopUnrollPass.h"
#include "llvm/Transforms/Scalar/SimpleLoopUnswitch.h"
#include "llvm/Transforms/Utils.h"
#include "llvm/Transforms/Vectorize.h"
#include "llvm/Transforms/Vectorize/LoopVectorize.h"
#include "llvm/Transforms/Vectorize/SLPVectorizer.h"

#include <string>


#define RUN_PASS(pass) \
    do {\
        pass(mod, result); \
    } while (0);

// inspired by https://github.com/klee/klee/blob/v2.3/lib/Module/Optimize.cpp#L83-L92
static inline void addPass(llvm::legacy::PassManager &PM, llvm::Pass *P) {
  // Add the pass to the pass manager...
  PM.add(P);

  PM.add(llvm::createVerifierPass());
}

void run_llvm_passes(llvm::Module *m) {
    auto &mod = *m;
    llvm::legacy::PassManager passes;
    

    // --loop-simplify --simplifycfg --loop-rotate --lcssa --licm --loop-unswitch --simplifycfg --instcombine --indvars
    // addPass(passes, llvm::createLoopInstSimplifyPass());
    addPass(passes,llvm::createLoopSimplifyPass ());
    // addPass(passes, llvm::createLoopSimplifyCFGPass());


    addPass(passes, llvm::createLoopRotatePass());
    // addPass(passes,llvm::createLoopSimplifyPass ());
    // addPass(passes, llvm::createLCSSAPass());
    // addPass(passes, llvm::createLICMPass());
    // addPass(passes, llvm::createSimpleLoopUnswitchLegacyPass());
    // // addPass(passes,llvm::createLoopUnswitchPass());
    // addPass(passes,llvm::createCFGSimplificationPass());
    // // addPass(passes,llvm::create());
    // addPass(passes,llvm::createIndVarSimplifyPass());
    // addPass(passes,llvm::createLoopIdiomPass());

    // addPass(passes,llvm::createLoopInterchangePass());
    // addPass(passes,);
    // addPass(passes,);
    // addPass(passes,);
    // addPass(passes,);
    if (passes.run(mod))
        std::cout << "modified\n";
    else
        std::cout << "nomodification\n";

    // llvm::createVerifierPass();

}

void printModule(llvm::Module *m) {
    std::string Str;
    llvm::raw_string_ostream OS(Str);
    OS << *m;
    OS.flush();
    std::cout << Str << std::endl;
}

Program ProgramParser::parse(const std::string& file, bool bitcastUnions) {
    Program result;
    llvm::LLVMContext context;

    auto error = llvm::SMDiagnostic();
    auto module = llvm::parseIRFile(file, error, context);
    if (!module) {
        throw std::invalid_argument("Error loading module - invalid input file:\n" + file + "\n");
    }

    // printModule(module.get());
    run_llvm_passes(module.get());
    const auto *mod = module.get();
    printModule(module.get());

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
    RUN_PASS(memcpyToAssignment);
    RUN_PASS(arrowify);
    RUN_PASS(deleteUnusedVariables);
    RUN_PASS(fixMainParameters);
    RUN_PASS(addSignCasts);
    // This pass is not working well. It does not take
    // into account the signdness of types and can
    // remove casts that are needed, e.g.:
    // It can do (int)var from (int)(short)var
    // where var is (unsigned short).
    // RUN_PASS(deleteRedundantCasts);
    RUN_PASS(extractVars);
    RUN_PASS(identifyInlinableBlocks);
    RUN_PASS(inlineBlocks);
    // llvm::errs() << "SECOND REDDEREF CALL" << "\n";
    // RUN_PASS(refDeref);


    return result;
}
