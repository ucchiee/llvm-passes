#include "TraverseCFG.h"
#include "llvm/ADT/BreadthFirstIterator.h"
#include "llvm/ADT/DepthFirstIterator.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/Twine.h"
#include "llvm/Analysis/PostDominators.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/CFG.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/Dominators.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/InstIterator.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/PassManager.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/raw_ostream.h"

//
// opt -load-pass-pluginbuild/FenceBeforeJCPass.so -passes="fence" test.ll -S
//
using namespace llvm;
void TraverseCFG::insertMod(
    Instruction *InsertBefore, Value *Lhs, Value *Rhs, bool Signed) {
  auto Ops =
      Signed ? Instruction::BinaryOps::SRem : Instruction::BinaryOps::URem;
  auto *binop = BinaryOperator::Create(
      Ops, Lhs, Rhs, Twine(), InsertBefore);
  errs() << "Inserting: " << *binop << "\n";
  // binop->insertBefore(InsertBefore);
}
void TraverseCFG::insertMax(Instruction *InsertBefore, Value *Sbj, Value *Min) {
}

void TraverseCFG::instrument(CmpInst *Cmp, BasicBlock *BB, bool isTrue) {
  errs() << "instrumenting BB that starts with " << BB->front() << "\n";
  // lhs Cmp rhs
  Value *lhs = Cmp->getOperand(0);
  Value *rhs = Cmp->getOperand(1);

  auto predicate = isTrue ? Cmp->getPredicate() : Cmp->getInversePredicate();

  switch (predicate) {
  case CmpInst::ICMP_EQ: {
    break;
  }
  case CmpInst::ICMP_NE: {
    break;
  }
  case CmpInst::ICMP_SGT:
  case CmpInst::ICMP_UGT: {
    // a > b
    if (!isa<ConstantInt>(*rhs)) {
      // b is a variable
      // b = b % a
      insertMod(&BB->front(), rhs, lhs, Cmp->isSigned());
    } else {
    }
    break;
  }
  case CmpInst::ICMP_SGE:
  case CmpInst::ICMP_UGE: {
    // a >= b
    if (!isa<ConstantInt>(*rhs)) {
      // b is a variable
      // b = b % (a + 1)
      if (isa<ConstantInt>(*lhs)) {
        // a is a constant
        auto *lhs_constant = cast<ConstantInt>(lhs);
        auto *lhs_plus_1 = ConstantInt::get(
            lhs_constant->getType(), lhs_constant->getValue() + 1);
        insertMod(&BB->front(), rhs, lhs_plus_1, Cmp->isSigned());
      }
    } else {
    }
    break;
  }
  case CmpInst::ICMP_SLT:
  case CmpInst::ICMP_ULT: {
    // a < b
    if (!isa<ConstantInt>(*lhs)) {
      // a is a variable
      insertMod(&BB->front(), lhs, rhs, Cmp->isSigned());
    } else {
    }
    break;
  }
  case CmpInst::ICMP_SLE:
  case CmpInst::ICMP_ULE: {
    // a <= b
    if (!isa<ConstantInt>(*lhs)) {
      // a is a variable
    } else {
    }
    break;
  }
  default: {
    // floating comparison is not supported
    break;
  }
  }
}

PreservedAnalyses TraverseCFG::run(Function &F, FunctionAnalysisManager &FAM) {
  // bfs
  // 分岐する際の制約を分岐先で課す
  for (BasicBlock *BB : breadth_first(&F)) {
    // Else 節が無いパターンの検出をこれで行う。
    // CFG を逆向きに見て Dominance を判定する。
    // Else 節が無い場合、
    // False 側の BB は True 側の BB を post-dominate する。
    PostDominatorTree PDT = PostDominatorTree(F);

    auto &I = BB->back();
    // Continue when `I` is not conditional branch
    if (!(isa<BranchInst>(I) && cast<BranchInst>(I).isConditional())) {
      continue;
    }
    auto &Br = cast<BranchInst>(I);
    auto *Cmp = cast<CmpInst>(Br.getOperand(0));
    auto *ThenBB = Br.getSuccessor(0);
    auto *ElseBB = Br.getSuccessor(1);

    errs() << "cmp: " << *Cmp << "\n";
    errs() << "br : " << Br << "\n";

    if (PDT.dominates(ElseBB, ThenBB)) {
      // Then, ElseBB is actually a fall-through BB (= There is no else
      // statement):
      // if (...) {
      //   ...
      // }
      errs() << "then only\n";
      instrument(Cmp, ThenBB, true);
    } else {
      // Then, ElseBB is a else BB:
      // if (...) {
      //   ...
      // } else {
      //   ...
      // }
      errs() << "if-else\n";
      instrument(Cmp, ThenBB, true);
      instrument(Cmp, ElseBB, false);
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
