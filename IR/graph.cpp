#include "graph.h"
#include "basicBlock.h"
#include <algorithm>

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

static void DFSPOBlockVisit(BasicBlock *block, std::set<BasicBlock *> &visited,
                            std::vector<BasicBlock *> &result) {
  if (visited.find(block) != visited.end() || block == nullptr) {
    return;
  }
  visited.insert(block);

  for (auto succ : block->GetSuccs()) {
    DFSPOBlockVisit(succ, visited, result);
  }
  result.push_back(block);
}

std::vector<BasicBlock *> MethodGraph::DFSPO(BasicBlock *startBlock) {
  std::set<BasicBlock *> visited;
  std::vector<BasicBlock *> result;
  if (startBlock == nullptr) {
    if (blocks_.empty()) {
      return result;
    }
    startBlock = blocks_[0];
  }

  DFSPOBlockVisit(startBlock, visited, result);
  return result;
}

std::vector<BasicBlock *> MethodGraph::RPO(BasicBlock *startBlock) {
  std::vector<BasicBlock *> result = DFSPO(startBlock);
  std::reverse(result.begin(), result.end());
  return result;
}
} // namespace ir