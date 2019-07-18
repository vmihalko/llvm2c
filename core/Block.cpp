
#include "Block.h"

#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/GetElementPtrTypeIterator.h"
#include "llvm/ADT/APInt.h"

#include "Func.h"
#include "../type/Type.h"
#include "../expr/BinaryExpr.h"
#include "../expr/UnaryExpr.h"

#include <utility>
#include <cstdint>
#include <string>
#include <set>
#include <iostream>
#include <fstream>
#include <regex>


Block::Block(const std::string &blockName, const llvm::BasicBlock* block, Func* func)
	: block(block),
	func(func),
	blockName(blockName), doInline(false) { }

bool Block::isVoidType(llvm::DITypeRef type) {
	if (llvm::DIDerivedType* dtype = llvm::dyn_cast_or_null<llvm::DIDerivedType>(type)) {
		if (!dtype->getBaseType()) {
			return true;
		}

		return isVoidType(dtype->getBaseType());
	}

	if (llvm::DICompositeType* ctype = llvm::dyn_cast_or_null<llvm::DICompositeType>(type)) {
		if (!ctype->getBaseType()) {
			return true;
		}

		return isVoidType(ctype->getBaseType());
	}

	return false;
}

void Block::addOwnership(std::unique_ptr<Expr> expr) {
    func->program->ownership.push_back(std::move(expr));
}

void Block::addExprAndOwnership(std::unique_ptr<Expr> expr) {
    addExpr(expr.get());
    addOwnership(std::move(expr));
}
