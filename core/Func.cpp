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
    auto it = exprMap.find(val);
	if (it == exprMap.end()) {
		if (auto F = llvm::dyn_cast<llvm::Function>(val)) {
			createExpr(val, std::make_unique<Value>("&" + F->getName().str(), getType(F->getReturnType())));
			return exprMap.find(val)->second.get();
		}
	} else {
		return it->second.get();
	}

	return program->getGlobalVar(val);
}

void Func::createExpr(const llvm::Value* val, std::unique_ptr<Expr> expr) {
	exprMap[val] = std::move(expr);
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

Struct* Func::getStruct(const llvm::StructType* strct) const {
	return program->getStruct(strct);
}

Struct* Func::getStruct(const std::string& name) const {
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

std::unique_ptr<Type> Func::getType(const llvm::Type* type) {
	return program->getType(type);
}

void Func::createPhiVariable(const llvm::Value* phi) {
	auto var = std::make_unique<Value>(getVarName() + "_phi", getType(phi->getType()));
	phiVariables.push_back(var.get());

	createExpr(phi, std::move(var));
}

void Func::addPhiAssignment(const llvm::Value* phi, const llvm::BasicBlock* inBlock, const llvm::Value* inValue) {
	phiEntries.push_back(PhiEntry{phi, inBlock, inValue});
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


void Func::output(std::ostream& stream) {

	if (isCFunc(name)) {
		if (name.compare("va_start") == 0
			|| name.compare("va_end") == 0
			|| name.compare("va_copy") == 0
			|| isCMath(name)) {
			return;
		}
	}

	if (true || program->includes) {
		if (isStdLibFunc(name) || isStringFunc(name) || isStdioFunc(name) || isPthreadFunc(name)) {
			return;
		}
	} else {
		//sometimes LLVM uses these functions with more arguments than their C counterparts
		if ((name.compare("memcpy") == 0 || name.compare("memset") == 0 || name.compare("memmove") == 0) && function->arg_size() > 3) {
			return;
		}
	}

	stream << returnType->toString();
	auto PT = dynamic_cast<PointerType*>(returnType.get());
	if (PT && PT->isArrayPointer) {
		stream << " (";
		for (unsigned i = 0; i < PT->levels; i++) {
			stream << "*";
		}
		stream << name << "(";
	} else {
		stream << " " << name << "(";
	}

	bool first = true;

    for (auto& val : parameters) {
		if (!first) {
			stream << ", ";
		}
		first = false;

		stream << val->getType()->toString();
		stream << " ";
		stream << val->toString();

		val->init = true;
    }

	if (isVarArg) {
		if (!first) {
			stream << ", ";
		}
		stream << "...";
	}

	stream << ")";

	if (PT && PT->isArrayPointer) {
		stream << ")" + PT->sizes;
	}

	if (isDeclaration) {
		stream << ";\n";
		return;
	}

	stream << " {\n";

	// start with variables from phi nodes
	for (const auto& var : phiVariables) {
		stream << "    ";
		stream << var->getType()->toString();
		stream << " ";
		stream << var->toString();
		stream << ";\n";
	}

	first = true;
	for (const auto& blockEntry : blockMap) {
		if (!first) {
			stream << blockEntry.second->blockName;
			stream << ":\n    ;\n";
		}
		blockEntry.second->output(stream);
		first = false;
	}

	stream << "}\n\n";
}


