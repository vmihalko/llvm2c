#include "Program.h"

#include <llvm/IR/LLVMContext.h>
#include "llvm/Support/raw_ostream.h"
#include "llvm/IRReader/IRReader.h"
#include "llvm/IR/Constants.h"

#include "../type/Type.h"

#include <fstream>
#include <exception>
#include <algorithm>
#include <regex>
#include <iostream>

Program::Program() : typeHandler(TypeHandler(this)) {
}

std::string Program::getAnonStructName() {
	std::string name = "anonymous_struct" + std::to_string(anonStructCount);
	anonStructCount++;
	return name;
}

void Program::unsetAllInit() {
	/* for (auto& gvar : globalVars) { */
	/* 	gvar->init = false; */
	/* } */

	/* for (auto& strct : structs) { */
	/* 	strct->isPrinted = false; */
	/* } */
}

Struct* Program::getStruct(const llvm::StructType* strct) const {
	std::string structName = TypeHandler::getStructName(strct->getName().str());

	for (const auto& structElem : structs) {
		if (structElem->name.compare(structName) == 0) {
			return structElem.get();
		}
	}

	if (unnamedStructs.find(strct) != unnamedStructs.end()) {
		return unnamedStructs.find(strct)->second.get();
	}

	return nullptr;
}

Struct* Program::getStruct(const std::string& name) const {
	for (const auto& structElem : structs) {
		if (structElem->name.compare(name) == 0) {
			return structElem.get();
		}
	}

	for (const auto& mapElem : unnamedStructs) {
		if (mapElem.second->name.compare(name) == 0) {
			return mapElem.second.get();
		}
	}

	return nullptr;
}

RefExpr* Program::getGlobalVar(const llvm::Value* val) {
	if (const llvm::GlobalVariable* GV = llvm::dyn_cast<llvm::GlobalVariable>(val)) {
		if (globalRefs.count(GV)) {
			return globalRefs.find(GV)->second.get();
		}
	}

	return nullptr;
}

void Program::addDeclaration(const llvm::Function* func, std::unique_ptr<Func> decl) {
	if (!isFunctionDeclared(func)) {
		declarations[func] = std::move(decl);
	}
}

void Program::createNewUnnamedStruct(const llvm::StructType *strct) {
	if (unnamedStructs.find(strct) != unnamedStructs.end()) {
		return;
	}

	auto structExpr = std::make_unique<Struct>(getAnonStructName());

	for (llvm::Type* type : strct->elements()) {
		structExpr->addItem(getType(type), getStructVarName());
	}

	unnamedStructs[strct] = std::move(structExpr);
}

std::unique_ptr<Type> Program::getType(const llvm::Type* type) {
	return typeHandler.getType(type);
}

std::string Program::getIncludeString() const {
	std::string ret;

	if (hasVarArg) {
		ret += "#include <stdarg.h>\n";
	}

	if (hasStdLib) {
		ret += "#include <stdlib.h>\n";
	}

	if (hasString) {
		ret += "#include <string.h>\n";
	}

	if (hasStdio) {
		ret += "#include <stdio.h>\n";
	}

	if (hasPthread) {
		ret += "#include <pthread.h>\n";
	}

	if (!ret.empty()) {
		ret += "\n";
	}

	return ret;
}

Func* Program::getFunction(const llvm::Function* f) {
	auto it = functions.find(f);
	if (it == functions.end()) {
		return nullptr;
	}

	return it->second.get();
}

RefExpr* Program::getGlobalRef(const llvm::GlobalVariable* gvar) {
	return globalRefs[gvar].get();
}

void Program::addFunction(const llvm::Function* llvmFunc, std::unique_ptr<Func> func) {
	functions[llvmFunc] = std::move(func);
}

void Program::addStruct(std::unique_ptr<Struct> strct) {
	structs.push_back(std::move(strct));
}

const std::set<std::string>& Program::getGlobalVarNames() const {
	return globalVarNames;
}

std::string Program::getStructVarName() {
	std::string varName = "structVar";
	varName += std::to_string(structVarCount);
	structVarCount++;

	return varName;
}

bool Program::isFunctionDeclared(const llvm::Function* func) const {
	return declarations.count(func) > 0;
}

void Program::output(std::ostream &stream) {
	/* unsetAllInit(); */

	/* stream << getIncludeString(); */

	/* if (!structs.empty()) { */
	/* 	stream << "//Struct declarations\n"; */
	/* 	for (auto& strct : structs) { */
	/* 		stream << "struct " << strct->name << ";\n"; */
	/* 	} */
	/* 	stream << "\n"; */
	/* } */

	/* if (typeHandler.hasTypeDefs()) { */
	/* 	stream << "//typedefs\n"; */
	/* 	for (auto elem : typeHandler.sortedTypeDefs) { */
	/* 		stream << elem->defToString() << "\n"; */
	/* 	} */
	/* 	stream << "\n"; */
	/* } */

	/* if (!structs.empty()) { */
	/* 	stream << "//Struct definitions\n"; */
	/* 	for (auto& strct : structs) { */
	/* 		if (!strct->isPrinted) { */
	/* 			outputStruct(strct.get(), stream); */
	/* 		} */
	/* 	} */
	/* 	stream << "\n"; */
	/* } */

	/* if (!unnamedStructs.empty()) { */
	/* 	stream << "//Anonymous struct declarations\n"; */
	/* 	for (const auto& elem : unnamedStructs) { */
	/* 		stream << "struct " << elem.second->name << ";\n"; */
	/* 	} */
	/* 	stream << "\n"; */
	/* } */

	/* if (!globalVars.empty()) { */
	/* 	stream << "//Global variable declarations\n"; */
	/* 	for (auto& gvar : globalVars) { */
	/* 		if (includes && (gvar->valueName.compare("stdin") == 0 || gvar->valueName.compare("stdout") == 0 || gvar->valueName.compare("stderr") == 0)) { */
	/* 			continue; */
	/* 		} */

	/* 		stream << gvar->declToString(); */
	/* 		stream << "\n"; */
	/* 	} */
	/* 	stream << "\n"; */
	/* } */

	/* if (!declarations.empty()) { */
	/* 	stream << "//Function declarations\n"; */
	/* 	for (const auto& func : declarations) { */
	/* 		func.second->output(stream); */
	/* 	} */
	/* 	stream << "\n"; */
	/* } */

	/* if (!unnamedStructs.empty()) { */
	/* 	stream << "//Anonymous struct definitions\n"; */
	/* 	for (auto& elem : unnamedStructs) { */
	/* 		if (!elem.second->isPrinted) { */
	/* 			outputStruct(elem.second.get(), stream); */
	/* 		} */
	/* 	} */
	/* 	stream << "\n"; */
	/* } */

	/* if (!globalVars.empty()) { */
	/* 	stream << "//Global variable definitions\n"; */
	/* 	for (auto& gvar : globalVars) { */
	/* 		if (includes && (gvar->valueName.compare("stdin") == 0 || gvar->valueName.compare("stdout") == 0 || gvar->valueName.compare("stderr") == 0)) { */
	/* 			gvar->init = true; */
	/* 			continue; */
	/* 		} */

	/* 		stream << gvar->toString(); */
	/* 		gvar->init = true; */
	/* 		stream << "\n"; */
	/* 	} */
	/* 	stream << "\n"; */
	/* } */

	/* stream << "//Function definitions\n"; */
	/* for (const auto& func : functions) { */
	/* 	func.second->output(stream); */
	/* } */
}

void Program::outputStruct(Struct* strct, std::ostream& stream) {
	/* for (auto& item : strct->items) { */
	/* 	if (auto AT = dynamic_cast<ArrayType*>(item.first.get())) { */
	/* 		if (AT->isStructArray) { */
	/* 			outputStruct(getStruct(AT->structName), stream); */
	/* 		} */
	/* 	} */

	/* 	if (auto PT = dynamic_cast<PointerType*>(item.first.get())) { */
	/* 		if (PT->isStructPointer && PT->isArrayPointer) { */
	/* 			outputStruct(getStruct(PT->structName), stream); */
	/* 		} */
	/* 	} */

	/* 	if (auto ST = dynamic_cast<StructType*>(item.first.get())) { */
	/* 		for (auto& s : structs) { */
	/* 			if (s->name == ST->name) { */
	/* 				outputStruct(s.get(), stream); */
	/* 			} */
	/* 		} */

	/* 		for (auto& s : unnamedStructs) { */
	/* 			if (s.second->name == ST->name) { */
	/* 				outputStruct(s.second.get(), stream); */
	/* 			} */
	/* 		} */
	/* 	} */
	/* } */

	/* if (!strct->isPrinted) { */
	/* 	stream << strct->toString(); */
	/* 	strct->isPrinted = true; */
	/* 	stream << "\n\n"; */
	/* } */
}

Func* Program::getDeclaration(const llvm::Function* func) {
    auto it = declarations.find(func);
    if (it == declarations.end()) {
        return nullptr;
    }

    return it->second.get();
}
