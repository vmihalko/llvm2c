#include "../core/Program.h"

#include <llvm/IR/Instruction.h>
#include <llvm/IR/CFG.h>

#include <iostream>

static bool isSafeToInline(const llvm::BasicBlock* llvmBlock, const Block* block) {
    // std::cout << "vinco:" << llvmBlock->getNumUses() << std::endl;
    // auto prdcssr = llvmBlock->getSinglePredecessor();
    // prdcssr->isLegalToHoistInto();
    // Is it a good idea to
    // if my pre

    // #1
    // if basic block has two predecessor
    // one of them is block itself
    // is safe to inline this block?

    // #2
    // if:
    //    1. b = function.getIndex( block ). (block[b-1]) is predecessor( block )
    //    2. numOfPredecessor == 2, 
    //    3. block.isPredecessorToItself()
    // 

    // #?
    // cycles had always at elast 2 predecessor
    // can cycles have more than 1 predecessor?

    // every loop is simple form has two predecessors
    // itself and preheader,
    // make loop inlinable - but not here, rather in parsing

    if (llvmBlock->getSinglePredecessor()) {
        // TODO If'm my predeccessor
        return true;
    }

    if (block->expressions.size() == 1) {
        const Expr* fst = block->expressions.front();
        return llvm::isa<RetExpr>(fst);
    }

    return false;
}

void identifyInlinableBlocks(const llvm::Module* module, Program& program) {
    assert(program.isPassCompleted(PassType::CreateExpressions));
    assert(program.isPassCompleted(PassType::ParseBreaks));
    // for (const llvm::Function& func : module->functions()) {
    //     auto* function = program.getFunction(&func);
    //     for (const auto& block : func) {
    //         auto* myBlock = function->createBlockIfNotExist(&block);
    //         std::cout << myBlock->blockName << "  " << myBlock->expressions.size() << std::endl;
    //         for (auto& expr : myBlock->expressions) {
    //             std::cout << expr->getKind() << std::endl;
    //             switch (expr->getKind())
    //             {
    //             case Expr::EK_AssignExpr:
    //                 std::cout << "ass\n";
    //                 break;
    //             case Expr::EK_GotoExpr:
    //                 // auto* gotoExpr = ;   
    //                 std::cout << "goto: " << llvm::dyn_cast_or_null<GotoExpr>(expr)->target->blockName << std::endl;
    //                 break;
    //             case Expr::EK_DoWhile:
    //                 std::cout << "dwhile\n";
    //                 break;
    //             case Expr::EK_RetExpr:
    //                 std::cout << "ret\n";
    //                 break;
    //             default:
    //                 break;
    //             }
    //             // }
    //             // if ( expr->getKind() == Expr::EK_)
    //             //     std::cout <<;
    //         }
    //     }
    // }    


    for (const llvm::Function& func : module->functions()) {
        auto* function = program.getFunction(&func);
        for (const auto& block : func) {
            auto* myBlock = function->createBlockIfNotExist(&block);
            if (!myBlock->doInline)
                myBlock->doInline = isSafeToInline(&block, myBlock);
        }

        if (func.begin() == func.end())
            continue;

        // first block is only inlinable if there are no predecessors
        // ??? missing if?
        auto& firstBlock = *func.begin();
        auto* myBlock = function->createBlockIfNotExist(&firstBlock);

        myBlock->doInline = false;
        myBlock->isFirst = true;
    }

    program.addPass(PassType::IdentifyInlinableBlocks);
}
