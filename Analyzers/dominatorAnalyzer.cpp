#include "dominatorAnalyzer.h"
#include "basicBlock.h"
#include "graph.h"
#include "util.h"
#include <algorithm>

namespace analyzer {

static void DFSPOBlockVisit(ir::BasicBlock *block,
                            std::set<ir::BasicBlock *> &visited,
                            std::vector<ir::BasicBlock *> &result,
                            ir::BasicBlock *ignoredBlock) {
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

std::vector<ir::BasicBlock *>
DominatorAnalyzer::DFSPO(ir::BasicBlock *startBlock,
                         ir::BasicBlock *ignoredBlock) {
  std::set<ir::BasicBlock *> visited;
  std::vector<ir::BasicBlock *> result;
  if (startBlock == nullptr) {
    if (graph_.blocks_.empty()) {
      return result;
    }
    startBlock = graph_.blocks_[0];
  }

  DFSPOBlockVisit(startBlock, visited, result, ignoredBlock);
  return result;
}

static std::vector<BlockWithIndex>
EnumerateBlocks(const std::vector<ir::BasicBlock *> &blocks) {
  std::vector<BlockWithIndex> result;
  size_t index = 0;
  for (auto block : blocks) {
    result.emplace_back(block, index++);
  }
  return std::move(result);
}

std::vector<BlockWithIndex>
DominatorAnalyzer::RPO(ir::BasicBlock *startBlock,
                       ir::BasicBlock *ignoredBlock) {
  std::vector<ir::BasicBlock *> result = DFSPO(startBlock, ignoredBlock);
  std::reverse(result.begin(), result.end());
  return std::move(EnumerateBlocks(result));
}

void DominatorAnalyzer::DumpRPO(ir::IrDumper &dumper,
                                ir::BasicBlock *startBlock,
                                ir::BasicBlock *ignoredBlock) {
  for (auto block : RPO(startBlock)) {
    dumper.Add(block.second);
    dumper.Add(". bb ");
    dumper.Add(block.first->GetId());
    dumper.Endl();
  }
}

std::set<ir::BasicBlock *>
CreateSetFromIndexedBlocks(const std::vector<BlockWithIndex> &indexedBlocks) {
  std::set<ir::BasicBlock *> result;
  for (auto block : indexedBlocks) {
    result.insert(block.first);
  }
  return std::move(result);
}

void DominatorAnalyzer::FindDominators(ir::BasicBlock *startBlock) {
  if (startBlock == nullptr) {
    if (graph_.blocks_.empty()) {
      return;
    }
    startBlock = graph_.blocks_[0];
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

void DominatorAnalyzer::DumpDominators(ir::IrDumper &dumper,
                                       ir::BasicBlock *startBlock) {
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

DominatorTree::DominatorTree(const ir::MethodGraph &graph,
                             ir::BasicBlock *startBlock)
    : graph_(graph) {
  if (startBlock == nullptr) {
    if (graph_.blocks_.empty()) {
      return;
    }
    startBlock = graph_.blocks_[0];
  }
  DominatorAnalyzer analyzer(graph);
  auto RPO = analyzer.RPO(startBlock);
  root_ = createNode(RPO[0], nullptr);
}

DominatorTree::TreeNode *DominatorTree::createNode(BlockWithIndex block,
                                                   TreeNode *parent) {
  auto node = new TreeNode(block, parent);
  if (parent != nullptr) {
    parent->children.push_back(node);
  }

  for (auto child : block.first->GetDominatedBlocks()) {
    if (child.first->GetIDominator() != block.first) {
      continue;
    }
    createNode(child, node);
  }
  return node;
}

static void DumpNode(ir::IrDumper &dumper, DominatorTree::TreeNode *node) {
  if (node == nullptr) {
    return;
  }

  dumper.Add(node->block.second);
  dumper.Add(". bb ");
  dumper.Add(node->block.first->GetId());
  if (!node->children.empty()) {
    dumper.IncreaseIndent();
    dumper.Endl();
    for (auto child : node->children) {
      DumpNode(dumper, child);
      if (child == node->children.back()) {
        dumper.DecreaseIndent();
        continue;
      }
      dumper.Endl();
    }
    return;
  }
}

void DominatorTree::Dump(ir::IrDumper &dumper) {
  dumper.Add("Dominator tree for graph: ");
  dumper.Add(graph_.GetName());
  dumper.IncreaseIndent();
  dumper.Endl();
  DumpNode(dumper, root_);
  dumper.DecreaseIndent();
}

} // namespace analyzer