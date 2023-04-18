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
#include <algorithm>
#include <optional>
#include <utility>






































// template <class GraphType>
// iterator_range<typename llvm::GraphTraits<GraphType>::ChildIteratorType>
// children(const typename llvm::GraphTraits<GraphType>::NodeRef &G) {
//   return make_range(GraphTraits<GraphType>::child_begin(G),
//                     GraphTraits<GraphType>::child_end(G));
// }



void myFirstPass(const llvm::Module* mod, Program& program) {

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
        llvm::LoopInfo &LI = FAM.getResult<llvm::LoopAnalysis>(const_cast<llvm::Function &>(function));
        for( const auto& loop : LI ) {
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
