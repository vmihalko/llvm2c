#pragma once

#include <vector>
#include <map>
#include <set>

#include "llvm/ADT/DenseMap.h"

class Program;

#include "../expr/Expr.h"
#include "../expr/UnaryExpr.h"
#include "../expr/BinaryExpr.h"
#include "Block.h"
#include "Program.h"

/**
 * @brief The Func class represents one of the functions of the LLVM program.
 */
class Func {
friend class Block;
friend class Program;

public:
    std::unique_ptr<Type> returnType;

    const llvm::Function* function;
    Program* program;

    Block* entry;

    std::map<const llvm::BasicBlock*, std::unique_ptr<Block>> blockMap; //DenseMap used for mapping llvm::BasicBlock to Block

    std::string name;

    std::vector<Value*> parameters;

    //set containing metadata names of variables (and names of global variables) that are in "var[0-9]+" format, used in creating variable names
    std::set<std::string> metadataVarNames;

    // variables that correspond to phi nodes and will be declared at the beginning of the function
    std::vector<Value*> phiVariables;

    //variables used for creating names for variables and blocks
    unsigned varCount = 0;
    unsigned blockCount = 0;

    bool isDeclaration; //function is only being declared
    bool isVarArg = false; //function has variable number of arguments

    Expr* lastArg; //last argument before variable arguments


    /**
     * @brief createNewUnnamedStruct
     * @param strct
     */
    void createNewUnnamedStruct(const llvm::StructType* strct);

    /**
     * @brief getMetadataNames Parses variable medatada in function and saves the variable names into the metadataVarNames set.
     */
    void getMetadataNames();

public:
    /**
     * @brief getBlockName Returns name of the block if the block already has an assigned name.
     * Otherwise assigns new name for the block in form of string containing block + blockCount,
     * inserts the block to the blockMap and return the assigned name.
     * @param block llvm::BasicBlock
     * @return Name assigned for given block.
     */
    std::string getBlockName(const llvm::BasicBlock* block); //RENAME

    /**
     * @brief Func Constructor for Func.
     * @param func llvm::Function for parsing
     * @param program Program to which function belongs
     * @param isDeclaration function is only being declared
     */
    Func(const llvm::Function* func, Program* program, bool isDeclaration);

    /**
     * @brief getStruct Returns pointer to the Struct corresponding to the given LLVM StructType.
     * @param strct LLVM StructType
     * @return Pointer to Struct expression if the struct is found, nullptr otherwise
     */
    Struct* getStruct(const llvm::StructType* strct) const;

    /**
     * @brief getStruct Returns pointer to the Struct with the given name.
     * @param name Name of the struct
     * @return Pointer to Struct expression if the struct is found, nullptr otherwise
     */
    Struct* getStruct(const std::string& name) const;

    /**
     * @brief getGlobalVar Returns corresponding refference to GlobalValue expression.
     * @param val llvm global variable
     * @return RefExpr expression
     */
    RefExpr* getGlobalVar(llvm::Value* val) const;

    /**
     * @brief stackIgnored Indicated that intrinsic stacksave/stackrestore was ignored.
     */
    void stackIgnored();

    /**
     * @brief getType Transforms llvm::Type into corresponding Type object
     * @param type llvm::Type for transformation
     * @return unique_ptr to corresponding Type object
     */
    std::unique_ptr<Type> getType(const llvm::Type* type);

    /**
     * @brief createBlockIfNotExist Creates a new block inside of this function that corresponds to @block
     * @param block LLVM BasicBlock to create a new block for
     */
    Block* createBlockIfNotExist(const llvm::BasicBlock* block);

    /**
     * @brief getExpr Finds Expr in exprMap or globalRefs with key val. If val is function, creates Value containing refference to the function and returns pointer to this Value.
     * @param val Key of the Expr
     * @return Pointer to the Expr if val is found, nullptr otherwise.
     */
    Expr* getExpr(const llvm::Value* val);

    /**
     * @brief createExpr Inserts expr into the exprMap using val as a key.
     * @param val Key
     * @param expr Mapped Value
     */
    void createExpr(const llvm::Value* val, std::unique_ptr<Expr> expr);

    /**
     * @brief getBlock Obtains a block from this function that corresponds to the specified LLVM block
     * @param block LLVM basic block
     * @return Pointer to the block if found, nullptr otherwise.
     */
    Block* getBlock(const llvm::BasicBlock* block);

    /**
     * @brief getVarName Creates a new name for a variable in form of string containing "var" + varCount.
     * @return String containing a variable name.
     */
    std::string getVarName();

    void setVarArg(bool va);

    template< typename T >
    void fillMetadataVarNames(const T& globalVarNames) {
        metadataVarNames.insert(globalVarNames.begin(), globalVarNames.end());
    }

    void addMetadataVarName(const std::string& varName);

    /**
     * @brief createPhiVariable Creates a new variable for @phi.
     */
    void createPhiVariable(const llvm::Value* phi);

    void output(std::ostream& out);

};
