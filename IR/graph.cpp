#include "graph.h"
#include "basicBlock.h"
#include "instructions/instr.h"
#include <algorithm>
#include <cstddef>

namespace ir {

BasicBlock *MethodGraph::AllocateBlock() {
  auto block = new BasicBlock(this, GetNextBlockId());
  blocks_.push_back(block);
  return block;
}

void MethodGraph::AddBlock(BasicBlock *block) { blocks_.push_back(block); }
bool MethodGraph::RemoveBlock(BasicBlock *block) {
  for (auto it = blocks_.begin(); it != blocks_.end(); ++it) {
    if (*it == block) {
      blocks_.erase(it);
      return true;
    }
  }
  return false;
}

MethodGraph::~MethodGraph() {
  for (auto block : blocks_) {
    if (block != nullptr) {
      delete block;
    }
  }
}

void MethodGraph::Dump(IrDumper &dumper) {
  dumper.Add("method graph: ");
  dumper.Add(name_);
  dumper.IncreaseIndent();
  dumper.Endl();
  for (auto block : blocks_) {
    block->Dump(dumper);
    dumper.Endl();
  }
  dumper.DecreaseIndent();
}

} // namespace ir