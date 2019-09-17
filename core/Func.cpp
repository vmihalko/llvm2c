#include "Func.h"

#include <llvm/IR/Module.h>
#include <llvm/IR/Metadata.h>
#include <llvm/Support/raw_ostream.h>

#include "../type/Type.h"
#include "../parser/cfunc.h"

#include <utility>
#include <cstdint>
#include <string>
#include <fstream>
#include <set>
#include <regex>

Func::Func(const llvm::Function* func, Program* program, bool isDeclaration) {
	this->program = program;
	function = func;
	this->isDeclaration = isDeclaration;
	returnType = getType(func->getReturnType());
}

Expr* Func::getExpr(const llvm::Value* val) {
    auto it = program->exprMap.find(val);
	if (it == program->exprMap.end()) {
		if (auto F = llvm::dyn_cast_or_null<llvm::Function>(val)) {
			createExpr(val, std::make_unique<Value>(F->getName().str(), getType(F->getReturnType())));
			return program->exprMap.find(val)->second;
		}
	} else {
		return it->second;
	}

	return program->getGlobalVar(val);
}

void Func::createExpr(const llvm::Value* val, std::unique_ptr<Expr> expr) {
	program->exprMap[val] = expr.get();
    program->ownership.push_back(std::move(expr));
}

std::string Func::getVarName() {
	std::string varName = "var";
	while (metadataVarNames.count(varName + std::to_string(varCount))) {
		varCount++;
	}
	varName += std::to_string(varCount);
	varCount++;

	return varName;
}

StructType* Func::getStruct(const llvm::StructType* strct) const {
	return program->getStruct(strct);
}

StructType* Func::getStruct(const std::string& name) const {
	return program->getStruct(name);
}

RefExpr* Func::getGlobalVar(llvm::Value* val) const {
	return program->getGlobalVar(val);
}

void Func::stackIgnored() {
	program->stackIgnored = true;
}

void Func::createNewUnnamedStruct(const llvm::StructType* strct) {
	program->createNewUnnamedStruct(strct);
}

Type* Func::getType(const llvm::Type* type) {
	return program->getType(type);
}

Expr* Func::createPhiVariable(const llvm::Value* phi) {
	auto var = std::make_unique<Value>(getVarName() + "_phi", getType(phi->getType()));
	phiVariables.push_back(var.get());

    return program->addOwnership(std::move(var));
}

Block* Func::createBlockIfNotExist(const llvm::BasicBlock* block) {
	auto iter = blockMap.find(block);
	Block *result = nullptr;
	if (iter == blockMap.end()) {
		std::string blockName = "block";
		blockName += std::to_string(blockCount);
		auto newBlock = std::make_unique<Block>(blockName, block, this);
		result = newBlock.get();
		blockMap[block] = std::move(newBlock);
		blockCount++;
	} else {
		result = iter->second.get();
	}
	return result;
}

Block* Func::getBlock(const llvm::BasicBlock* block) {
	auto iter = blockMap.find(block);
	if (iter == blockMap.end()) {
		return nullptr;
	}
	return iter->second.get();
}

void Func::setVarArg(bool va) {
	isVarArg = va;
}

void Func::addMetadataVarName(const std::string& varName) {
	metadataVarNames.insert(varName);
}
