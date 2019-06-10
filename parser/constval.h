#pragma once

#include <llvm/IR/Instruction.h>

#include "../core/Func.h"
#include "../core/Block.h"

void createConstantValue(const llvm::Value* val, Func* func, Block* block);
