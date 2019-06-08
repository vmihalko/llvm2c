#pragma once

#include <llvm/IR/Instruction.h>
#include "../core/Program.h"

void parseGlobalVars(const llvm::Module* module, Program& program);
void parseStructs(const llvm::Module* module, Program& program);
void parseFunctions(const llvm::Module* module, Program& program);
void determineIncludes(const llvm::Module* module, Program& program);
void findMetadataNames(const llvm::Module* module, Program& program);
void createFunctions(const llvm::Module* module, Program& program);
void createFunctionParameters(const llvm::Module* module, Program& program);
void createBlocks(const llvm::Module* module, Program& program);
void createAllocas(const llvm::Module* module, Program& program);
void parseMetadataTypes(const llvm::Module* module, Program& program);
void createExpressions(const llvm::Module* module, Program& program);
void findDeclaredFunctions(const llvm::Module* module, Program& program);
void nameFunctions(const llvm::Module* module, Program& program);
