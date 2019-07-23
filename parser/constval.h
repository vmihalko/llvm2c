#pragma once

#include <llvm/IR/Instruction.h>

#include "../core/Func.h"
#include "../core/Block.h"

Expr* createConstantValue(const llvm::Value* val, Program& program);

Expr* createUndefValue(const llvm::Type* ty, Program& program);
