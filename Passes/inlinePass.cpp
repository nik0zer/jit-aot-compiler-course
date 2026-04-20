#include "inlinePass.h"
#include "basicBlock.h"
#include "graph.h"
#include "instructions/callStaticInstr.h"
#include "instructions/instr.h"
#include "instructions/paramInstr.h"
#include "instructions/returnInstr.h"
#include "macro.h"
#include <algorithm>
#include <tuple>
#include <vector>

namespace passes {
namespace {

void ChangeParamsUses(ir::BasicBlock *entryBlock, ir::instr::Instr *callInstr) {
  for (auto instr = entryBlock->GetFirstInstr(); instr != nullptr;
       instr = instr->GetNextInstr()) {
    if (instr->IsParamInstr()) {
      auto paramInstr = instr->AsParamInstr();
      auto paramIndex = paramInstr->GetParamId();
      auto correspondingInput = callInstr->GetInputs()[paramIndex];
      auto users = paramInstr->GetUsers();
      for (auto user : users) {
        user->ReplaceInput(paramInstr, correspondingInput);
      }
    }
  }
}

void ChangeReturnUses(ir::BasicBlock *exitBlock, ir::instr::Instr *callInstr) {
  ASSERT(exitBlock->GetLastInstr()->IsReturnInstr());
  auto retInstr = exitBlock->GetLastInstr()->AsReturnInstr();

  if (retInstr == nullptr) {
    UNREACHABLE();
    return;
  }

  auto users = callInstr->GetUsers();
  for (auto user : users) {
    user->ReplaceInput(callInstr, retInstr);
  }
}

} // namespace

ir::BasicBlock *InlinePass::SplitBlock(ir::MethodGraph *callerGraph,
                                       ir::BasicBlock *bb,
                                       ir::instr::Instr *callInstr) {
  auto newBlock = callerGraph->AllocateBlockAfter(bb);
  auto succs = bb->GetSuccs();
  newBlock->SetSuccs(succs);
  bb->SetSuccs({newBlock, nullptr});
  newBlock->SetPreds({bb});

  for (auto succ : newBlock->GetSuccs()) {
    if (succ == nullptr) {
      continue;
    }
    auto &preds = succ->GetPreds();
    for (size_t i = 0; i < preds.size(); ++i) {
      if (preds[i] == bb) {
        succ->SetPred(newBlock, i);
        break;
      }
    }
  }

  auto firstToMove = callInstr->GetNextInstr();
  if (firstToMove != nullptr) {
    auto oldLast = bb->last_;
    bb->last_ = callInstr;
    callInstr->SetNextInstr(nullptr);

    newBlock->first_ = firstToMove;
    newBlock->last_ = oldLast;
    firstToMove->SetPrevInstr(nullptr);

    newBlock->firstNonPhi_ = firstToMove;
  }
  for (auto instr = newBlock->GetFirstInstr(); instr != nullptr; instr = instr->GetNextInstr()) {
    instr->SetBB(newBlock);
  }
  return newBlock;
}

void InlinePass::TryInlineStaticCall(ir::MethodGraph *callerGraph,
                                     ir::instr::CallStaticInstr *callInstr,
                                     const std::string &methodName) {
  auto *bb = callInstr->GetBB();
  auto calleeGraph = methodGraphGenerator_(methodName);
  if (calleeGraph == nullptr || calleeGraph->GetBlocks().empty()) {
    return;
  }

  size_t instrCount = 0;
  for (auto block : calleeGraph->GetBlocks()) {
    for (auto instr = block->GetFirstInstr(); instr != nullptr;
         instr = instr->GetNextInstr()) {
      instrCount++;
    }
  }

  if (instrCount > MAX_NUMBER_OF_INSTRS_TO_INLINE) {
    return;
  }

  for (auto block : calleeGraph->GetBlocks()) {
    block->SetId(callerGraph->GetNextBlockId());
    for (auto instr = block->GetFirstInstr(); instr != nullptr;
         instr = instr->GetNextInstr()) {
      instr->SetInstrId(callerGraph->GetNextInstrId());
    }
  }

  auto newBlock = SplitBlock(callerGraph, bb, callInstr);

  auto entryBlock = calleeGraph->GetBlocks()[0];
  auto exitBlock = calleeGraph->GetBlocks().back();
  ChangeParamsUses(entryBlock, callInstr);
  ChangeReturnUses(exitBlock, callInstr);

  bb->SetSuccs({entryBlock, nullptr});
  entryBlock->SetPreds({bb});

  exitBlock->SetSuccs({newBlock, nullptr});
  newBlock->SetPreds({exitBlock});

  auto callerBlocks = callerGraph->GetBlocks();
  std::vector<ir::BasicBlock *> newCallerBlocks;
  newCallerBlocks.reserve(callerBlocks.size() +
                          calleeGraph->GetBlocks().size());

  auto insertPosIt = std::find(callerBlocks.begin(), callerBlocks.end(), bb);

  for (auto it = callerBlocks.begin(); it != insertPosIt; ++it) {
    newCallerBlocks.push_back(*it);
  }
  newCallerBlocks.push_back(*insertPosIt);

  const auto &calleeBlocks = calleeGraph->GetBlocks();
  newCallerBlocks.insert(newCallerBlocks.end(), calleeBlocks.begin(),
                         calleeBlocks.end());

  for (auto it = insertPosIt + 1; it != callerBlocks.end(); ++it) {
    newCallerBlocks.push_back(*it);
  }

  callerGraph->SetBlocks(std::move(newCallerBlocks));
  calleeGraph->SetBlocks({});

  delete calleeGraph;
}

void InlinePass::Run(ir::MethodGraph *graph) {
  std::vector<
      std::tuple<ir::instr::CallStaticInstr *, std::string>>
      callSites;
  for (auto bb : graph->GetBlocks()) {
    for (auto instr = bb->GetFirstNonPhiInstr(); instr != nullptr;
         instr = instr->GetNextInstr()) {
      if (instr->GetOpcode() == ir::instr::InstrOpcode::CALL_STATIC) {
        auto callInstr = instr->AsCallStaticInstr();
        callSites.emplace_back(callInstr, callInstr->MethodName());
      }
    }
  }

  for (const auto &site : callSites) {
    TryInlineStaticCall(graph, std::get<0>(site),
                        std::get<1>(site));
  }
}

} // namespace passes
