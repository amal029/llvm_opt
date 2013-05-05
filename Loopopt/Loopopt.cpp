#define DEBUG_TYPE "avinash"
#include "llvm/ADT/Statistic.h"
#include "llvm/IR/Function.h"
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/Analysis/LoopPass.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/Instructions.h"
#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Analysis/AliasAnalysis.h"
#include "llvm/Analysis/AliasSetTracker.h"
#include "llvm/Analysis/Dominators.h"
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/Analysis/LoopIterator.h"
#include "llvm/Analysis/LoopPass.h"
#include "llvm/Analysis/ScalarEvolution.h"
#include "llvm/Analysis/ScalarEvolutionExpander.h"
#include "llvm/Analysis/ScalarEvolutionExpressions.h"
#include "llvm/Analysis/TargetTransformInfo.h"
#include "llvm/Analysis/ValueTracking.h"
#include "llvm/Analysis/DependenceAnalysis.h"
#include "llvm/Analysis/Verifier.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/IntrinsicInst.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Value.h"
#include "llvm/Pass.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/PatternMatch.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Target/TargetLibraryInfo.h"
#include "llvm/Transforms/Scalar.h"
#include "llvm/Transforms/Utils/BasicBlockUtils.h"
#include "llvm/Transforms/Utils/Local.h"
#include <algorithm>
#include <map>
using namespace llvm;

STATISTIC(LoopBlockCounter, "Counts number of loops greeted");

namespace {
  // Loopopt - The first implementation, without getAnalysisUsage.
  struct LoopBlock : public LoopPass {
    static char ID; // Pass identification, replacement for typeid
    LoopBlock() : LoopPass(ID) {}

    virtual bool runOnLoop(Loop *L, LPPassManager &LPM) {
      ScalarEvolution *SE;
      DataLayout *DL;
      LoopInfo *LI;
      TargetTransformInfo *TTI;
      DominatorTree *DT;
      AliasAnalysis *AA;
      TargetLibraryInfo *TLI;
      DependenceAnalysis *DA;

      SE = &getAnalysis<ScalarEvolution>();
      DL = getAnalysisIfAvailable<DataLayout>();
      LI = &getAnalysis<LoopInfo>();
      DA = &getAnalysis<DependenceAnalysis>();
      TTI = &getAnalysis<TargetTransformInfo>();
      DT = &getAnalysis<DominatorTree>();
      AA = getAnalysisIfAvailable<AliasAnalysis>();
      TLI = getAnalysisIfAvailable<TargetLibraryInfo>();

      ++LoopBlockCounter;
      errs() << "Hello from loop: " << LoopBlockCounter << "\n";
      // Try iterating through the blocks
      for (Loop::block_iterator bb = L->block_begin(),
	     be = L->block_end(); bb != be; ++bb) {
	for(BasicBlock::iterator j=(*bb)->begin(); j!=(*bb)->end();){
	  Instruction *instr = j++;
	  // Find out if the operands are of type vector!
	  for(unsigned i=0;i<instr->getNumOperands();++i){
	    switch(instr->getOperand(i)->getType()->getTypeID()){
	    case Type::VectorTyID: {
	      // errs() << "The instruction: ";
	      // instr->print(errs());
	      // errs () << " has vector operands\n";
	    } break;
	    default: break;
	    }
	  }
	}
	// Another example to calculate the input condition for the loop
	BasicBlock *pre_header = L->getLoopPredecessor();
	pre_header->print(errs());
	BasicBlock *latch = L->getLoopLatch();
	latch->print(errs());
      }
      return false;
    }
    virtual void getAnalysisUsage(AnalysisUsage &AU) const {
      LoopPass::getAnalysisUsage(AU);
      // Required means I need these passes before hand!!
      AU.addRequiredID(LoopSimplifyID);
      AU.addRequiredID(LCSSAID);
      AU.addRequired<DominatorTree>();
      AU.addRequired<LoopInfo>();
      AU.addRequired<DependenceAnalysis>();
      AU.addRequired<ScalarEvolution>();
      AU.addRequired<TargetTransformInfo>();
      // Preserved means I do not invalidate these passes!!
      AU.addPreserved<LoopInfo>();
      AU.addPreserved<DominatorTree>();
      AU.addPreserved<DependenceAnalysis>();

      // Add other passes that one might need!!
    }
  };
}

char LoopBlock::ID = 0;
static RegisterPass<LoopBlock> Y("loop-block", "Block and strip-mine loops");
