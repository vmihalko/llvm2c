#include <llvm/IR/Instruction.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/DebugInfoMetadata.h>

#include "../core/Program.h"
#include "../core/Func.h"
#include "../core/Block.h"

#include <iostream>
#include <functional>

static void declareFunc(const llvm::Function* func, Program& program, const llvm::Module* module) {
    auto decl = std::make_unique<Func>(func, &program, func->isDeclaration());
    for (const llvm::Value& arg : func->args()) {
        auto argVal = std::make_unique<Value>(decl->getVarName(), program.getType(arg.getType()));
        decl->createExpr(&arg, std::move(argVal));
    }

    if (func->args().begin() != func->args().end()) {
        decl->setVarArg(func->isVarArg());
    }

    // ========================================================================
    // GENERAL FIX: Adjust function signatures based on debug info and usage
    // ========================================================================
    // 
    // PROBLEM IN LLVM IR:
    // - Function defined as: define i8* @allocator()  [type: i8* ()]
    // - Used as: bitcast (i8* ()* @allocator to i8* (i8*)*)
    // - Target function pointer type: i8* (i8*)* expects one parameter
    // - Source function type: i8* () has zero parameters
    //
    // WHY THIS HAPPENS:
    // - In C, K&R style functions (no parameter list) are compatible with
    //   function pointers that expect parameters (legacy C compatibility)
    // - LLVM IR allows bitcast between these pointer types (they're both pointers)
    // - Original C: void *allocator() is compatible with void*(*)(void*)
    //
    // THE ISSUE:
    // - llvm2c generates: void* allocator(void);  [based on function definition]
    // - But when used as function pointer, needs: void* allocator(void*);
    // - CBMC's function pointer checking requires exact signature match
    //
    // GENERAL SOLUTION (in priority order):
    // 1. Check debug info (DISubprogram) - most authoritative source
    //    - If debug info has more parameters than LLVM definition, use debug info
    // 2. Fall back to scanning function pointer usage (bitcasts)
    //    - Find all places where function is used as function pointer
    //    - Collect all target function pointer types it's cast to
    //    - If any target has more parameters, adjust function declaration to match
    // ========================================================================
    
    // Get the function's actual type from LLVM
    const llvm::FunctionType* funcType = func->getFunctionType();
    unsigned currentParamCount = funcType->getNumParams();
    unsigned targetParamCount = currentParamCount;
    const llvm::FunctionType* targetFuncType = nullptr;
    
    // PRIORITY 1: Check debug info (DISubprogram) - most authoritative
    if (const llvm::DISubprogram* subprogram = func->getSubprogram()) {
        if (const llvm::DISubroutineType* diSubroutineType = 
            llvm::dyn_cast_or_null<llvm::DISubroutineType>(subprogram->getType())) {
            // DISubroutineType::getTypeArray() returns [returnType, param1, param2, ...]
            // So size - 1 = number of parameters
            unsigned debugParamCount = diSubroutineType->getTypeArray().size() - 1;
            
            // If debug info has more parameters than LLVM definition, use debug info
            if (debugParamCount > currentParamCount) {
                targetParamCount = debugParamCount;
                // We'll need to add parameters based on debug info types
                // But we need the actual LLVM types, so we'll use function pointer scanning
                // as a fallback to get the types
            }
        }
    }
    
    // PRIORITY 2: Scan function pointer usage (bitcasts) to find target types
    // This is needed when:
    // - Debug info doesn't exist or doesn't help (K&R style functions)
    // - We need the actual LLVM types for the parameters
    unsigned maxParamCountFromUsage = currentParamCount;
    
    // Helper to check if a value is a bitcast of our function to a function pointer
    std::function<void(const llvm::Value*)> checkBitcast = [&](const llvm::Value* val) {
        const llvm::Value* operand = nullptr;
        const llvm::Type* destType = nullptr;
        
        // Check if it's a BitCast instruction
        if (const llvm::BitCastInst* BC = llvm::dyn_cast<llvm::BitCastInst>(val)) {
            operand = BC->getOperand(0);
            destType = BC->getDestTy();
        }
        // Check if it's a ConstantExpr bitcast (common in call arguments)
        else if (const llvm::ConstantExpr* CE = llvm::dyn_cast<llvm::ConstantExpr>(val)) {
            if (CE->getOpcode() == llvm::Instruction::BitCast) {
                operand = CE->getOperand(0);
                destType = CE->getType();
            }
        }
        
        // Extract the function from the operand
        // In LLVM IR, for bitcast (i8* ()* @allocator to i8* (i8*)*):
        // - operand is @allocator, which is a Function* (GlobalValue)
        // - In ConstantExpr, @allocator is a Constant that is the Function* itself
        if (!operand || !destType) {
            return; // Skip if we don't have valid operand or destination type
        }
        
        const llvm::Function* operandFunc = nullptr;
        // Try direct cast first (operand might be Function* directly)
        if (const llvm::Function* F = llvm::dyn_cast<llvm::Function>(operand)) {
            operandFunc = F;
        }
        // If operand is a GlobalValue (which includes Function), try casting
        // In LLVM, function references in constants are Function* directly
        else if (const llvm::GlobalValue* GV = llvm::dyn_cast<llvm::GlobalValue>(operand)) {
            operandFunc = llvm::dyn_cast<llvm::Function>(GV);
        }
        
        // If this bitcast involves our function
        if (operandFunc == func && destType) {
            // Check if destination is a function pointer type
            if (const llvm::PointerType* PT = llvm::dyn_cast<llvm::PointerType>(destType)) {
                if (const llvm::FunctionType* FT = 
                    llvm::dyn_cast<llvm::FunctionType>(PT->getPointerElementType())) {
                    // Found a cast to function pointer type - check parameter count
                    unsigned targetParamCount = FT->getNumParams();
                    if (targetParamCount > maxParamCountFromUsage) {
                        maxParamCountFromUsage = targetParamCount;
                        targetFuncType = FT;
                    }
                }
            }
        }
    };
    
    // Scan all instructions in all functions
    for (const auto& F : module->functions()) {
        for (const auto& BB : F) {
            for (const auto& I : BB) {
                checkBitcast(&I);
                // Also check operands of call instructions (bitcasts are often in call args)
                if (const llvm::CallInst* CI = llvm::dyn_cast<llvm::CallInst>(&I)) {
                    // CallInst operands: [0] = called function, [1..N] = arguments
                    for (unsigned i = 1; i < CI->getNumOperands(); i++) {
                        checkBitcast(CI->getOperand(i));
                    }
                }
            }
        }
    }
    
    // Scan global variable initializers (constant expressions)
    for (const auto& GV : module->globals()) {
        if (GV.hasInitializer()) {
            checkBitcast(GV.getInitializer());
        }
    }
    
    // Determine final parameter count: use the maximum from debug info or usage
    unsigned finalParamCount = std::max(targetParamCount, maxParamCountFromUsage);
    
    // If we need to add parameters, use the function pointer type we found
    // (it has the actual LLVM types we need)
    if (finalParamCount > currentParamCount && targetFuncType) {
        for (unsigned i = currentParamCount; i < finalParamCount; i++) {
            llvm::Type* paramType = targetFuncType->getParamType(i);
            Type* cParamType = program.getType(paramType);
            auto paramVal = std::make_unique<Value>(decl->getVarName(), cParamType);
            decl->parameters.push_back(paramVal.get());
            program.addOwnership(std::move(paramVal));
        }
    }

    // FIXME: a hack for CPAchecker -- do this optional
    if (func->getName().startswith("__VERIFIER_nondet_") &&
        (func->getName().endswith("_int") || 
         func->getName().endswith("_long") || 
         func->getName().endswith("_short") || 
         func->getName().endswith("_char") || 
         func->getName().endswith("_float") ||
         func->getName().endswith("_double"))) {
        decl->returnType = program.typeHandler.setSigned(static_cast<IntegerType*>(decl->returnType));
    }
    program.addFunction(func, std::move(decl));
}

void findDeclaredFunctions(const llvm::Module *module, Program& program) {
    for(const llvm::Function& func : module->functions()) {
        if (func.hasName()) {
            if (func.isDeclaration() || llvm::Function::isInternalLinkage(func.getLinkage())) {
                if (func.getName().str().substr(0, 8) != "llvm.dbg" && func.getName() != "isnan") {
                    if (!program.isFunctionDeclared(&func)) {
                        declareFunc(&func, program, module);
                    }
                }
            }

            if (!func.isDeclaration()) {
                if (!program.isFunctionDeclared(&func)) {
                    declareFunc(&func, program, module);
                }
            }
        }
    }

    program.addPass(PassType::FindDeclaredFunctions);
}
