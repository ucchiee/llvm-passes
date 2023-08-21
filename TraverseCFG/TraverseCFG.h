#ifndef FENCE_BEFORE_JC_H
#define FENCE_BEFORE_JC_H
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/PassManager.h"
#include <set>

namespace llvm {
class TraverseCFG : public PassInfoMixin<TraverseCFG> {
public:
  PreservedAnalyses run(Function &F, FunctionAnalysisManager &FAM);
  static bool isRequired() { return true; }
};
} // namespace llvm

#endif
