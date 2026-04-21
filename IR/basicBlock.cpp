#include "basicBlock.h"
#include "instructions/phiInstr.h"
#include <algorithm>

namespace ir {
void BasicBlock::RemoveSuccessor(size_t index) {
  if (index >= MAX_NUM_OF_SUCCESSORS) {
    return;
  }

  auto succ = succs_[index];
  if (succ != nullptr) {
    size_t predIndex = -1;
    auto &preds = succ->GetPreds();
    for (size_t i = 0; i < preds.size(); ++i) {
      if (preds[i] == this) {
        predIndex = i;
        break;
      }
    }

    if (predIndex != -1 && succ->GetFirstInstr() != nullptr &&
        succ->GetFirstInstr()->IsPhiInstr()) {
      for (auto phi = succ->GetFirstInstr(); phi != nullptr;
           phi = (phi->GetNextInstr() && phi->GetNextInstr()->IsPhiInstr())
                     ? phi->GetNextInstr()->AsPhiInstr()
                     : nullptr) {
        phi->EraseInput(predIndex);
      }
    }
    succ->RemovePredecessor(this);
  }

  if (index == 0) {
    succs_[0] = succs_[1];
  }
  succs_[1] = nullptr;
}

void BasicBlock::RemovePredecessor(BasicBlock *pred) {
  preds_.erase(std::remove(preds_.begin(), preds_.end(), pred), preds_.end());
}

void BasicBlock::DumpPredecessors(IrDumper &dumper) {
  for (auto block : preds_) {
    if (block == nullptr) {
      continue;
    }
    dumper.Add("bb");
    dumper.Add(block->GetId());
    if (block != preds_.back()) {
      dumper.Add(", ");
    }
  }
}

template <typename Iterator>
static bool CheckNonNullElementInArray(Iterator startIterator,
                                       Iterator endIterator) {
  for (auto it = startIterator; it != endIterator; ++it) {
    if (*it != nullptr) {
      return true;
    }
  }
  return false;
}
void BasicBlock::DumpSuccessors(IrDumper &dumper) {
  for (auto it = succs_.begin(); it != succs_.end(); ++it) {
    auto block = *it;
    if (block == nullptr) {
      continue;
    }
    dumper.Add("bb");
    dumper.Add(block->GetId());
    if (std::next(it) == succs_.end()) {
      break;
    }
    if (CheckNonNullElementInArray(std::next(it), succs_.end())) {
      dumper.Add(", ");
    }
  }
}

void BasicBlock::Dump(IrDumper &dumper, bool dumpLiveness) {
  DumpPredecessors(dumper);
  if (!preds_.empty()) {
    dumper.Add(" -> ");
  }
  if (dumpLiveness) {
    dumper.Add("[ ");
    dumper.Add(liveRange_.first);
    dumper.Add(" : ");
    dumper.Add(liveRange_.second);
    dumper.Add(" ] ");
  }
  dumper.Add("basic block ");
  dumper.Add(id_);
  if (!succs_.empty()) {
    dumper.Add(" -> ");
  }
  DumpSuccessors(dumper);
  dumper.IncreaseIndent();
  dumper.Endl();
  auto instr = first_;
  while (instr != nullptr) {
    instr->Dump(dumper, dumpLiveness);
    instr = instr->GetNextInstr();
    dumper.Endl();
  }
  dumper.DecreaseIndent();
}

} // namespace ir