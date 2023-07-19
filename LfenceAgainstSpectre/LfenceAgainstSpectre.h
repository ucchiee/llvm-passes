#ifndef LFENCE_AGAINST_SPECTRE_H
#define LFENCE_AGAINST_SPECTRE_H
#include "llvm/IR/PassManager.h"

namespace llvm {
class LfenceAgainstSpectre : public PassInfoMixin<LfenceAgainstSpectre> {
public:
  PreservedAnalyses run(Function &F, FunctionAnalysisManager &FAM);
  static bool isRequired() { return true; }
};
} // namespace llvm

#endif
