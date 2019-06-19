
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

void Block::unsetAllInit() {
    /* for (auto expr : expressions) { */
    /*     if (Value* val = dynamic_cast<Value*>(expr)) { */
    /*         val->init = false; */
    /*     } */
    /* } */
}

bool Block::isVoidType(llvm::DITypeRef type) {
	if (llvm::DIDerivedType* dtype = llvm::dyn_cast<llvm::DIDerivedType>(type)) {
		if (!dtype->getBaseType()) {
			return true;
		}

		return isVoidType(dtype->getBaseType());
	}

	if (llvm::DICompositeType* ctype = llvm::dyn_cast<llvm::DICompositeType>(type)) {
		if (!ctype->getBaseType()) {
			return true;
		}

		return isVoidType(ctype->getBaseType());
	}

	return false;
}

void Block::insertValue(const llvm::Value* value, std::unique_ptr<Value> expr) {
	valueMap[value] = std::move(expr);
}

Value* Block::getValue(const llvm::Value* value) {
	return valueMap[value].get();
}

void Block::addValue(std::unique_ptr<Value> value) {
	values.push_back(std::move(value));
}

void Block::output(std::ostream& stream) {
	/* unsetAllInit(); */
	/* for (const auto& expr : expressions) { */

	/* 	if (auto V = dynamic_cast<Value*>(expr)) { */
	/* 		stream << "    "; */
	/* 		if (!V->init) { */
	/* 			stream << V->getType()->toString(); */
	/* 			stream << " "; */
	/* 			stream << expr->toString(); */
	/* 			stream << ";\n"; */
	/* 			V->init = true; */
	/* 		} */
	/* 		continue; */
	/* 	} */

	/* 	if (auto CE = dynamic_cast<CallExpr*>(expr)) { */
	/* 		if (func->program->noFuncCasts) { */
	/* 			auto call = CE->funcValue; */
	/* 			bool hasCast = false; */
	/* 			while (auto CAST = dynamic_cast<CastExpr*>(call)) { */
	/* 				hasCast = true; */
	/* 				call = CAST->expr; */
	/* 			} */

	/* 			if (hasCast) { */
	/* 				stream << "    "; */
	/* 				stream << call->toString().substr(1, call->toString().size() - 1); */
	/* 				stream << "(" << CE->paramsToString() << ");\n"; */
	/* 				continue; */
	/* 			} */
	/* 		} */
	/* 	} */

	/* 	if (auto EE = dynamic_cast<AssignExpr*>(expr)) { */
	/* 		if (func->program->noFuncCasts) { */
	/* 			if (auto CE = dynamic_cast<CallExpr*>(EE->right)) { */
	/* 				auto call = CE->funcValue; */
	/* 				bool hasCast = false; */
	/* 				while (auto CAST = dynamic_cast<CastExpr*>(call)) { */
	/* 					hasCast = true; */
	/* 					call = CAST->expr; */
	/* 				} */

	/* 				if (hasCast) { */
	/* 					stream << "    ("; */
	/* 					stream << EE->left->toString(); */
	/* 					stream << ") = "; */
	/* 					stream << call->toString().substr(1, call->toString().size() - 1); */
	/* 					stream << "(" << CE->paramsToString() << ");\n"; */
	/* 					continue; */
	/* 				} */
	/* 			} */
	/* 		} */
	/* 	} */

	/* 	stream << "    "; */
	/* 	stream << expr->toString(); */
	/* 	stream << "\n"; */
	/* } */
}
