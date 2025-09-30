#include "basicBlock.h"

namespace ir {
void BasicBlock::DumpRelativeBlocks(IrDumper &dumper,
                                    const std::vector<BasicBlock *> &blocks) {
  for (auto block : blocks) {
    dumper.Add("bb");
    dumper.Add(block->GetId());
    if (block != blocks.back()) {
      dumper.Add(", ");
    }
  }
}

void BasicBlock::Dump(IrDumper &dumper) {
  DumpRelativeBlocks(dumper, preds_);
  if (!preds_.empty()) {
    dumper.Add(" -> ");
  }
  dumper.Add("basic block ");
  dumper.Add(id_);
  if (!succs_.empty()) {
    dumper.Add(" -> ");
  }
  DumpRelativeBlocks(dumper, succs_);
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