#include "graph.h"
#include "basicBlock.h"

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
} // namespace ir