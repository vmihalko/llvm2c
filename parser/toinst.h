#pragma once

#include <llvm/IR/Constants.h>
#include <llvm/IR/Instruction.h>

#include <memory>
#include <functional>

/*
 * ConstantExpr is commonly transformed to Instruction using getAsInstruction.
 * The function, however, creates a new Instruction that is not part of the
 * program and causes issues. This function wraps the problematic Instruction in
 * a unique_ptr, so programmer won't forget to delete it.

   Example usage (in memcpyToAssign):
    if (auto* CE = llvm::dyn_cast_or_null<llvm::ConstantExpr>(val)) {
        auto inst = gai(CE);
        return stripCast(inst.get());
    }
 */
std::unique_ptr<llvm::Instruction, std::function<void(llvm::Instruction*)>> toInst(llvm::ConstantExpr* ce);
