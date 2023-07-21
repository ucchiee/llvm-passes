#include "X86LFenceBeforeJC.h"
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
// opt -load-pass-pluginbuild/X86LFenceBeforeJCPass.so -passes="fence" test.ll
// -S
//

using namespace llvm;

PreservedAnalyses X86LFenceBeforeJC::run(
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

    // Insert InlineAsm before `I`
    builder.SetInsertPoint(&I);
    auto *callInst = builder.CreateCall(IA);
    callInst->addFnAttr(Attribute::NoUnwind);
  }
  return PreservedAnalyses::all();
}

extern "C" ::llvm::PassPluginLibraryInfo LLVM_ATTRIBUTE_WEAK
llvmGetPassPluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, "X86LFenceBeforeJC", "v0.1",
      [](PassBuilder &PB) {
        // using OptimizationLevel= typename PassBuilder::OptimizationLevel;
        using PipelineElement = typename PassBuilder::PipelineElement;
        PB.registerPipelineEarlySimplificationEPCallback(
            [&](ModulePassManager &MPM, auto) {
              MPM.addPass(
                  createModuleToFunctionPassAdaptor(X86LFenceBeforeJC()));
              return true;
            });
      }};
}
