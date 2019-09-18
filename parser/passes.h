#pragma once

#include <llvm/IR/Instruction.h>
#include "../core/Program.h"

void initializeGlobalVars(const llvm::Module* module, Program& program);
void parseStructDeclarations(const llvm::Module* module, Program& program);
void parseStructItems(const llvm::Module* module, Program& program);
void parseFunctions(const llvm::Module* module, Program& program);
void determineIncludes(const llvm::Module* module, Program& program);
void findMetadataFunctionNames(const llvm::Module* module, Program& program);
void findMetadataVariableNames(const llvm::Module* module, Program& program);
void createFunctions(const llvm::Module* module, Program& program);
void createFunctionParameters(const llvm::Module* module, Program& program);
void createBlocks(const llvm::Module* module, Program& program);
void createAllocas(const llvm::Module* module, Program& program);
void parseMetadataTypes(const llvm::Module* module, Program& program);
void createExpressions(const llvm::Module* module, Program& program);
void findDeclaredFunctions(const llvm::Module* module, Program& program);
void nameFunctions(const llvm::Module* module, Program& program);
void parseBreaks(const llvm::Module* module, Program& program);
void addPhis(const llvm::Module* module, Program& program);
void identifyInlinableBlocks(const llvm::Module* module, Program& program);
void refDeref(const llvm::Module* mod, Program& program);
void fixMainParameters(const llvm::Module* module, Program& program);
void addSignCasts(const llvm::Module* module, Program& program);
void deleteRedundantCasts(const llvm::Module* module, Program& program);
void deleteUnusedVariables(const llvm::Module* module, Program& program);
void extractVars(const llvm::Module* module, Program& program);
void arrowify(const llvm::Module* module, Program& program);
void memcpyToAssignment(const llvm::Module* module, Program& program);
void computeGlobalVarsOrder(const llvm::Module* module, Program& program);
void createConstants(const llvm::Module* mod, Program& program);
void inlineBlocks(const llvm::Module* mod, Program& program);
