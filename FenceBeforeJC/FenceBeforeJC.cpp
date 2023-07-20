#include "FenceBeforeJC.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/Dominators.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/InlineAsm.h"
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

PreservedAnalyses FenceBeforeJC::run(
    Function &F, FunctionAnalysisManager &FAM) {
  IRBuilder<> builder(F.getContext());

  // Create InlineAsm
  StringRef asmString = "lfence";
  StringRef constraints = "";
  Type *Ty = Type::getVoidTy(F.getContext());
  FunctionType *FTy = FunctionType::get(Ty, false);
  InlineAsm *IA = InlineAsm::get(FTy, asmString, constraints, false);

  for (auto &I : instructions(F)) {
    // Continue when `I` is not conditional branch
    if (!(isa<BranchInst>(I) && cast<BranchInst>(I).isConditional())) {
      continue;
    }

    // Insert FenceInst before `I`
    // SSID : SyncScope::System
    //   Acquire -> no fence instruction
    //   AcquireRelease -> no fence instruction
    //   LAST -> mfence (x64)
    //   Monotonic -> compile error
    //   NotAtomic -> compile error
    //   Release -> no fence instruction
    //   SequentiallyConsistent -> mfence (x64)
    //   Unordered -> compile error
    // new FenceInst(F.getContext(), AtomicOrdering::LAST, SyncScope::System,
    // &I);

    // Insert InlineAsm before `I`
    builder.SetInsertPoint(&I);
    auto *callInst = builder.CreateCall(IA);
    callInst->addFnAttr(Attribute::NoUnwind);
  }
  return PreservedAnalyses::all();
}

extern "C" ::llvm::PassPluginLibraryInfo LLVM_ATTRIBUTE_WEAK
llvmGetPassPluginInfo() {
  return {
      LLVM_PLUGIN_API_VERSION, "FenceBeforeJC", "v0.1", [](PassBuilder &PB) {
        // using OptimizationLevel= typename PassBuilder::OptimizationLevel;
        using PipelineElement = typename PassBuilder::PipelineElement;
        PB.registerPipelineParsingCallback(
            [](StringRef Name, FunctionPassManager &FPM,
                ArrayRef<PipelineElement>) {
              if (Name == "fence") {
                FPM.addPass(FenceBeforeJC());
                return true;
              }
              return false;
            });
      }};
}
