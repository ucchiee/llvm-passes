#ifndef FENCE_BEFORE_JC_H
#define FENCE_BEFORE_JC_H
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/IR/PassManager.h"
#include <set>

namespace llvm {
class TraverseCFG : public PassInfoMixin<TraverseCFG> {
public:
  void instrument(CmpInst *Cmp, BasicBlock *BB, bool isTrue);
  void insertMod(
      Instruction *InsertBefore, Value *Lhs, Value *Rhs, bool Signed);
  void insertMax(Instruction *InsertBefore, Value *Sbj, Value *Min);
  PreservedAnalyses run(Function &F, FunctionAnalysisManager &FAM);
  static bool isRequired() { return true; }
};
} // namespace llvm

#endif
