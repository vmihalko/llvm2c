#pragma once

#include "llvm/ADT/DenseMap.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Value.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DebugInfoMetadata.h"
#include "llvm/IR/InlineAsm.h"

#include "../expr/Expr.h"

class Func;

/**
 * @brief The Block class represents one of the block of the LLVM function.
 */
class Block {
    friend class Func;
public:
    const llvm::BasicBlock* block;

    Func* func;

    // TODO move everything owned by the block into this vector
    std::vector<std::unique_ptr<Expr>> ownership;

    // a sequence of expression forming this basic block
    std::vector<Expr*> expressions;

    std::vector<std::unique_ptr<StackAlloc>> allocas;

    //getelementptr expressions
    std::vector<std::unique_ptr<GepExpr>> geps; //GepExpr vector used in getelementptr parsing
    std::vector<std::unique_ptr<Expr>> casts; //Vector of casted values used in parsing getelementptr and store instruction

    //store expressions
    std::map<Expr*, std::unique_ptr<Expr>> derefs; //Map used for storing unique pointers to DerefExpr (used in store instruction parsing)

    //alloca expressions
    llvm::DenseMap<const llvm::Value*, std::unique_ptr<Value>> valueMap; //map of Values used in parsing alloca instruction

    //extractvalue expressions
    std::vector<std::unique_ptr<Value>> values; //Vector of Values used in parsing extractvalue

    //inline asm and load expressions
    std::vector<std::unique_ptr<Expr>> vars; //Vector of Values used in parsing inline asm and load
    std::vector<std::unique_ptr<Expr>> stores; //Vector of EqualsExpr used in parsing inline asm and load
    std::vector<std::unique_ptr<Expr>> loadDerefs; //Vector of DerefExpr used in parsing load

    //call expressions
    std::vector<std::unique_ptr<Expr>> callExprMap; //Vector of CallExpr used in parsing call instruction
    std::vector<std::unique_ptr<Expr>> callValueMap; //Vector of EqualsExpr used in parsing call instruction

    // assignments of values to variables for phi nodes
    std::vector<std::unique_ptr<Expr>> phiAssignments;

    // instead of `goto block`, the block will be outputed in place
    bool doInline;

    /**
     * @brief createConstantValue Creates Value for given ConstantInt or ConstantFP and inserts it into exprMap.
     * @param val constant value
     */
    void createConstantValue(const llvm::Value* val);

    /**
     * @brief isVoidType Parses metadata about variable type. Returns wether the type is void or not.
     * @param type Metadata information about type
     * @return True if type is void, false otherwise.
     */
    bool isVoidType(llvm::DITypeRef type);

    /**
     * @brief createFuncCallParam Creates new Expr for parameter of function call.
     * @param param Parameter of function call
     */
    void createFuncCallParam(const llvm::Use& param);

    /**
     * @brief getAsmOutputString Parses asm constraint string to get output operands.
     * @param info ConstraintInfoVector containing parsed asm constraint string
     * @return Strings containing output operand for inline assembler
     */
    std::vector<std::string> getAsmOutputStrings(llvm::InlineAsm::ConstraintInfoVector info) const;

    /**
     * @brief getAsmInputStrings Parses asm constraint string to get input operands.
     * @param info ConstraintInfoVector containing parsed asm constraint string
     * @return Vector of strings containing input operand for inline assembler
     */
    std::vector<std::string> getAsmInputStrings(llvm::InlineAsm::ConstraintInfoVector info) const;

    /**
     * @brief getRegisterString Parses string containing register label from LLVM to C.
     * @param str LLVM register string
     * @return C register string
     */
    std::string getRegisterString(const std::string& str) const;

    /**
     * @brief getAsmUsedRegString Parses asm constraint string to get used registers.
     * @param info ConstraintInfoVector containing parsed asm constraint string
     * @return String containing used registers
     */
    std::string getAsmUsedRegString(llvm::InlineAsm::ConstraintInfoVector info) const;

    /**
     * @brief toRawString Converts string to its raw format (including escape chars etc.)
     * @param str String
     * @return String in raw format
     */
    std::string toRawString(const std::string& str) const;

    /**
     * @brief unsetAllInit Unsets the init flag in every Value in abstractSyntaxTree. Used for repeated calling of print or saveFile.
     */
    void unsetAllInit();

    /**
     * @brief addPhiAssignment Adds assignment @expr to the end of block (just before the last br)
     */
    void addPhiAssignment(std::unique_ptr<Expr> expr);

public:
    std::string blockName;

    void addExpr(Expr *expr) {
        expressions.push_back(expr);
    }

    /**
     * @brief Block Constructor for Block.
     * @param blockName Name of the block
     * @param block llvm::BasicBlock for parsing
     * @param func Func where the block is located
     */
    Block(const std::string &blockName, const llvm::BasicBlock* block, Func* func);

    /**
     * @brief output Outputs the translated block to the given stream.
     * @param stream Stream for output
     */
    void output(std::ostream& stream);

    void insertValue(const llvm::Value* value, std::unique_ptr<Value> expr);

    Value* getValue(const llvm::Value* value);

    void addValue(std::unique_ptr<Value> value);
};
