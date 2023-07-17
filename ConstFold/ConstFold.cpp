#include "llvm/ADT/SmallVector.h"
#include "llvm/IR/BasicBlock.h"
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
// opt -load-pass-plugin ./build/LLVMConstFoldPass.so -passes="const-fold" test.ll -S
//

using namespace llvm;

namespace {
class ConstOpt : public PassInfoMixin<ConstOpt> {
  std::vector<Instruction *> constAdds;
  void findConstAdds(Function &F);
  void constantFold(Instruction *I);

public:
  PreservedAnalyses run(Function &F, FunctionAnalysisManager &FAM);
  static bool isRequired() { return true; }
};
} // end anonymous namespace

PreservedAnalyses ConstOpt::run(Function &F, FunctionAnalysisManager &FAM) {
  auto PA = PreservedAnalyses::all();

  findConstAdds(F);

  while (constAdds.size() != 0) {
    for (auto *I : constAdds) {
      constantFold(I);
    }
    findConstAdds(F);
  }

  return PA;
}

void ConstOpt::findConstAdds(Function &F) {
  constAdds.clear();
  for (auto &I : instructions(F)) {
    if (I.getOpcode() == Instruction::Add) {
      if (isa<ConstantInt>(I.getOperand(0)) &&
          isa<ConstantInt>(I.getOperand(1))) {
        constAdds.push_back(&I);
      }
    }
  }
}

void ConstOpt::constantFold(Instruction *I) {
  auto *lhs = cast<ConstantInt>(I->getOperand(0));
  auto *rhs = cast<ConstantInt>(I->getOperand(1));

  // lhs and rhs is guaranteed to be not null
  auto ty = I->getType();
  auto result = lhs->getValue() + rhs->getValue();
  auto instVal = ConstantInt::get(ty, result);
  I->replaceAllUsesWith(instVal);
  I->eraseFromParent();
}

extern "C" ::llvm::PassPluginLibraryInfo LLVM_ATTRIBUTE_WEAK
llvmGetPassPluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, "ConstOpt", "v0.1", [](PassBuilder &PB) {
            // using OptimizationLevel= typename PassBuilder::OptimizationLevel;
            using PipelineElement = typename PassBuilder::PipelineElement;
            PB.registerPipelineParsingCallback(
                [](StringRef Name, FunctionPassManager &FPM,
                    ArrayRef<PipelineElement>) {
                  if (Name == "const-fold") {
                    FPM.addPass(ConstOpt());
                    return true;
                  }
                  return false;
                });
          }};
}
