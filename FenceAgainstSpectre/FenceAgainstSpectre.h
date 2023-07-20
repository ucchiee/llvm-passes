#ifndef FENCE_AGAINST_SPECTRE_H
#define FENCE_AGAINST_SPECTRE_H
#include "llvm/IR/PassManager.h"

namespace llvm {
class FenceAgainstSpectre : public PassInfoMixin<FenceAgainstSpectre> {
public:
  PreservedAnalyses run(Function &F, FunctionAnalysisManager &FAM);
  static bool isRequired() { return true; }
};
} // namespace llvm

#endif
