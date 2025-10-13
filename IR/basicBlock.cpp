#include "basicBlock.h"

namespace ir {
void BasicBlock::DumpPredecessors(IrDumper &dumper) {
  for (auto block : preds_) {
    dumper.Add("bb");
    dumper.Add(block->GetId());
    if (block != preds_.back()) {
      dumper.Add(", ");
    }
  }
}

template<typename Iterator>
static bool CheckNonNullElementInArray(Iterator startIterator, Iterator endIterator) {
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
    if(block == nullptr) {
      continue;
    }
    dumper.Add("bb");
    dumper.Add(block->GetId());
    if (std::next(it) == succs_.end()) {
      break;
    }
    if(CheckNonNullElementInArray(std::next(it), succs_.end())) {
      dumper.Add(", ");
    }
  }
}

void BasicBlock::Dump(IrDumper &dumper) {
  DumpPredecessors(dumper);
  if (!preds_.empty()) {
    dumper.Add(" -> ");
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
    instr->Dump(dumper);
    instr = instr->GetNextInstr();
    dumper.Endl();
  }
  dumper.DecreaseIndent();
}

} // namespace ir