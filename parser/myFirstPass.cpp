#include "../core/Program.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/DenseSet.h"
#include "llvm/ADT/GraphTraits.h"
#include "llvm/ADT/SmallPtrSet.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/IR/CFG.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/PassManager.h"
#include "llvm/Pass.h"
#include "llvm/Support/Allocator.h"
// #include "llvm/include/llvm/Transforms/Vectorize/LoopVectorizationLegality.h"
#include "llvm/Transforms/Vectorize/LoopVectorizationLegality.h"
#include <algorithm>
#include <optional>
#include <utility>






































// template <class GraphType>
// iterator_range<typename llvm::GraphTraits<GraphType>::ChildIteratorType>
// children(const typename llvm::GraphTraits<GraphType>::NodeRef &G) {
//   return make_range(GraphTraits<GraphType>::child_begin(G),
//                     GraphTraits<GraphType>::child_end(G));
// }

#include "llvm/Analysis/IVDescriptors.h"
#include "llvm/IR/Instructions.h"

using namespace llvm;

bool analyzeLoop(Loop *L, ScalarEvolution *SE) {
  for (BasicBlock *BB : L->blocks()) {
    for (Instruction &I : *BB) {
      if (PHINode *Phi = dyn_cast<PHINode>(&I)) {
        InductionDescriptor ID;
        // ID.is
    llvm::errs() << "a\n";
        if (InductionDescriptor::isInductionPHI(Phi, L, SE, ID)) {
          errs() << "Found induction variable: " << *Phi << "\n";
          errs() << "Step value: " << *ID.getStep() << "\n";
          errs() << "Start value: " << *ID.getStartValue() << "\n";
          return true;
        }
      }
    }
  }
  return false;
}

void myFirstPass(const llvm::Module* mod, Program& program) {
    (void)program;

    // llvm::ScalarEvolutionAnalysis SE;
    // passBuilder.se
    llvm::PassBuilder passBuilder;
	llvm::FunctionAnalysisManager FAM;
	llvm::LoopAnalysisManager LAM;
	llvm::CGSCCAnalysisManager CGAM;
	llvm::ModuleAnalysisManager MAM;

    passBuilder.registerModuleAnalyses(MAM);
	passBuilder.registerFunctionAnalyses(FAM);
	passBuilder.registerCGSCCAnalyses(CGAM);
	passBuilder.registerLoopAnalyses(LAM);


	passBuilder.crossRegisterProxies(LAM, FAM, CGAM, MAM);
    for (const auto& function : mod->functions()) {
        if(function.isIntrinsic() || function.isDeclaration()) continue;
        llvm::errs() << "in fun: " << function.getName() << "\n";
        // auto* func = program.getFunction(&function);
        // FAM. run(function, MAM);
        llvm::LoopInfo &LI = FAM.getResult<llvm::LoopAnalysis>(const_cast<llvm::Function &>(function));
        llvm::ScalarEvolution &SE = FAM.getResult<llvm::ScalarEvolutionAnalysis>(const_cast<llvm::Function &>(function));
        // if (&SE) {
        //     errs() << "ScalarEvolutionAnalysis has run on function: " << function.getName() << "\n";
        // } else {
        //     errs() << "ScalarEvolutionAnalysis has NOT run on function: " << function.getName() << "\n";
        // }
        for( const auto& loop : LI ) {
        if (loop->getInductionVariable(SE)) {
                        llvm::errs() << "Ind Var exists\n";
                        // phiNode is an induction variable
                        // You can use 'phiNode' here
                        // ...
                    }
        llvm::errs() << "analoopON\n";
        if (analyzeLoop(loop, &SE)) {
           loop->setLoopAlreadyUnrolled();
            // llvm::BasicBlock *forLoopHeader = loop->getHeader();
            // llvm::Function* forLoopParent = loop->getHeader()->getParent();
            // if (program.forLoopHeaders.count( forLoopParent ))
            //      program.forLoopHeaders[forLoopParent] = {};
            
            // //save for later link
            // program.forLoopHeaders[forLoopParent].push_back( forLoopHeader );
            
            // loop->getHeader()->getParent()->removeFromParent(); //unlink
            llvm::errs() << "FORLOOP-noop\n";
        }
        llvm::errs() << "analoopOFF\n";
            // FOR LOOP == (header is exiting and loop has induction variable)
            // bool lHeaderIsExiting = loop->isLoopExiting( loop->getHeader() );
            // for (auto& inst : *loop->getHeader()) {
            //     if (llvm::dyn_cast<llvm::PHINode>(&inst)) {
            //         // Check if the PHI node is used as an induction variable
            //         // auto* phiNode = 
            //         // loop->getInductionVariable(loop->getHeader()->getParent()->getScalarEve);

            //     }
            // }
            llvm::errs() << "LOOP has " <<  loop->getBlocks().size() << " number of blocks\n";
            for (unsigned i = 0; i < loop->getBlocks().size(); ++i) {
                llvm::BasicBlock *block = loop->getBlocks()[i];
                if (loop->isLoopExiting(block)){
                    llvm::errs() << *block << " is exiting\n";
                    for (const auto *Succ : llvm::children<const llvm::BasicBlock *>(block)) {
                        llvm::errs() << "***" << *Succ << "$\n";
                    }
                }
            }
        }
            // TODO: if loop is forLoop, remove parent=>skipRotate pass for such a loop...
            // let's see, if we have to consider nested loops, or other thing,
            // in other words, what exaclty we broke with such a change...
    }
}
