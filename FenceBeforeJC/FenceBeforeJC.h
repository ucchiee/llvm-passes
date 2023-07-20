#ifndef FENCE_BEFORE_JC_H
#define FENCE_BEFORE_JC_H
#include "llvm/IR/PassManager.h"

namespace llvm {
class FenceBeforeJC : public PassInfoMixin<FenceBeforeJC> {
public:
  PreservedAnalyses run(Function &F, FunctionAnalysisManager &FAM);
  static bool isRequired() { return true; }
};
} // namespace llvm

#endif
