#include "TraverseCFG.h"
#include "llvm/ADT/BreadthFirstIterator.h"
#include "llvm/ADT/DepthFirstIterator.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/CFG.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/Dominators.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/InstIterator.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/PassManager.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/raw_ostream.h"

//
// opt -load-pass-pluginbuild/FenceBeforeJCPass.so -passes="fence" test.ll -S
//

using namespace llvm;
PreservedAnalyses TraverseCFG::run(Function &F, FunctionAnalysisManager &FAM) {
  // bfs
  // 分岐する際の制約を分岐先で課す
  for (BasicBlock *BB : breadth_first(&F)) {
    auto &I = BB->back();
    // Continue when `I` is not conditional branch
    if (!(isa<BranchInst>(I) && cast<BranchInst>(I).isConditional())) {
      continue;
    }
  }
  return PreservedAnalyses::all();
}

extern "C" ::llvm::PassPluginLibraryInfo LLVM_ATTRIBUTE_WEAK
llvmGetPassPluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, "TraverseCFG", "v0.1", [](PassBuilder &PB) {
            // using OptimizationLevel= typename PassBuilder::OptimizationLevel;
            PB.registerPipelineEarlySimplificationEPCallback(
                [&](ModulePassManager &MPM, auto) {
                  MPM.addPass(createModuleToFunctionPassAdaptor(TraverseCFG()));
                  return true;
                });
          }};
}
