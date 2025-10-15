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

static void DFSPOBlockVisit(BasicBlock *block, std::set<BasicBlock *> &visited,
                            std::vector<BasicBlock *> &result,
                            BasicBlock *ignoredBlock) {
  if (visited.find(block) != visited.end() || block == nullptr ||
      block == ignoredBlock) {
    return;
  }
  visited.insert(block);

  for (auto succ : block->GetSuccs()) {
    DFSPOBlockVisit(succ, visited, result, ignoredBlock);
  }
  result.push_back(block);
}

std::vector<BasicBlock *> MethodGraph::DFSPO(BasicBlock *startBlock,
                                             BasicBlock *ignoredBlock) {
  std::set<BasicBlock *> visited;
  std::vector<BasicBlock *> result;
  if (startBlock == nullptr) {
    if (blocks_.empty()) {
      return result;
    }
    startBlock = blocks_[0];
  }

  DFSPOBlockVisit(startBlock, visited, result, ignoredBlock);
  return result;
}

static std::vector<BlockWithIndex>
EnumerateBlocks(const std::vector<BasicBlock *> &blocks) {
  std::vector<BlockWithIndex> result;
  size_t index = 0;
  for (auto block : blocks) {
    result.emplace_back(block, index++);
  }
  return std::move(result);
}

std::vector<BlockWithIndex> MethodGraph::RPO(BasicBlock *startBlock,
                                             BasicBlock *ignoredBlock) {
  std::vector<BasicBlock *> result = DFSPO(startBlock, ignoredBlock);
  std::reverse(result.begin(), result.end());
  return std::move(EnumerateBlocks(result));
}

void MethodGraph::DumpRPO(IrDumper &dumper, BasicBlock *startBlock,
                          BasicBlock *ignoredBlock) {
  for (auto block : RPO(startBlock)) {
    dumper.Add(block.second);
    dumper.Add(". bb ");
    dumper.Add(block.first->GetId());
    dumper.Endl();
  }
}

std::set<BasicBlock *>
CreateSetFromIndexedBlocks(const std::vector<BlockWithIndex> &indexedBlocks) {
  std::set<BasicBlock *> result;
  for (auto block : indexedBlocks) {
    result.insert(block.first);
  }
  return std::move(result);
}

void MethodGraph::FindDominators(BasicBlock *startBlock) {
  if (startBlock == nullptr) {
    if (blocks_.empty()) {
      return;
    }
    startBlock = blocks_[0];
  }

  auto indexedBlocks = RPO(startBlock);
  auto allBlocksSet = CreateSetFromIndexedBlocks(indexedBlocks);

  for (auto block : indexedBlocks) {
    std::vector<BlockWithIndex> dominatedBlocks{};
    auto liveBlocks = CreateSetFromIndexedBlocks(RPO(startBlock, block.first));
    for (auto checkBlock : indexedBlocks) {
      if (std::find(liveBlocks.begin(), liveBlocks.end(), checkBlock.first) ==
              liveBlocks.end() &&
          checkBlock.first != block.first) {
        dominatedBlocks.push_back(checkBlock);
      }
    }

    for (auto dominatedBlock : dominatedBlocks) {
      dominatedBlock.first->dominators_.push_back(block);
    }

    block.first->SetDominatedBlocks(dominatedBlocks);
  }

  for (auto block : indexedBlocks) {
    if (block.first->dominators_.empty()) {
      continue;
    }
    auto maxDominator = std::max_element(
        block.first->dominators_.begin(), block.first->dominators_.end(),
        [](const BlockWithIndex &a, const BlockWithIndex &b) {
          return a.second < b.second;
        });
    block.first->SetIDominator(maxDominator->first);
  }
}

void MethodGraph::DumpDominators(IrDumper &dumper, BasicBlock *startBlock) {
  auto indexedBlocks = RPO(startBlock);
  FindDominators(startBlock);

  for (auto block : indexedBlocks) {
    dumper.Add(block.second);
    dumper.Add(". bb ");
    dumper.Add(block.first->GetId());
    dumper.Add(" has dominators: ");
    for (auto dominator : block.first->GetDominators()) {
      dumper.Add("bb ");
      dumper.Add(dominator.first->GetId());
      if (dominator != block.first->GetDominators().back()) {
        dumper.Add(", ");
      }
    }
    dumper.Add("; ");
    if (block.first->GetIDominator() != nullptr) {
      dumper.Add("idominator: ");
      dumper.Add(block.first->GetIDominator()->GetId());
    }
    dumper.Add("; ");
    dumper.Add("dominator for: ");
    for (auto dominatedBlock : block.first->GetDominatedBlocks()) {
      dumper.Add("bb ");
      dumper.Add(dominatedBlock.first->GetId());
      if (dominatedBlock != block.first->GetDominatedBlocks().back()) {
        dumper.Add(", ");
      }
    }
    dumper.Endl();
  }
}

} // namespace ir