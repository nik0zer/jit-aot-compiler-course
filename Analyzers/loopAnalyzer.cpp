#include "loopAnalyzer.h"
#include "basicBlock.h"
#include "irDumper.h"
#include <algorithm>

namespace analyzer {

LoopAnalyzer::~LoopAnalyzer() {
  for (auto block : graph_->GetBlocks()) {
    if (block != nullptr) {
      block->SetLoop(nullptr);
    }
  }

  for (auto *loop : reducibleLoops_) {
    delete loop;
  }
  reducibleLoops_.clear();

  for (auto *loop : irreducibleLoops_) {
    delete loop;
  }
  irreducibleLoops_.clear();
}

void LoopAnalyzer::FindLoops(ir::BasicBlock *startBlock, bool dominatorsFound) {
  if (startBlock == nullptr) {
    if (graph_->GetBlocks().empty()) {
      return;
    }
    startBlock = graph_->GetBlocks()[0];
  }

  if (!dominatorsFound) {
    dominatorAnalyzer_.FindDominators(startBlock);
  }

  cachedRpo_ = dominatorAnalyzer_.RPO(startBlock);
  blockToNodeMap_.clear();
  for (auto &node : cachedRpo_) {
    blockToNodeMap_[node.first] = &node;
  }

  CollectBackEdges(startBlock);
  PopulateLoops();
}

void LoopAnalyzer::CollectBackEdges(ir::BasicBlock *startBlock) {
  std::set<ir::BasicBlock *> markedWithGrey;
  std::set<ir::BasicBlock *> markedWithBlack;
  CollectBackEdgesDFSHelper(startBlock, markedWithGrey, markedWithBlack);
}

void LoopAnalyzer::CollectBackEdgesDFSHelper(
    ir::BasicBlock *block, std::set<ir::BasicBlock *> &markedWithGrey,
    std::set<ir::BasicBlock *> &markedWithBlack) {

  if (markedWithBlack.find(block) != markedWithBlack.end()) {
    return;
  }

  markedWithGrey.insert(block);

  for (auto succ : block->GetSuccs()) {
    if (succ == nullptr) {
      continue;
    }

    if (markedWithGrey.find(succ) != markedWithGrey.end()) {
      bool isDominated = false;
      const auto &dominators = block->GetDominators();

      for (const auto &dom : dominators) {
        if (dom.first == succ) {
          isDominated = true;
          break;
        }
      }

      if (isDominated) {
        // reducible loop
        if (succ->GetLoop() == nullptr) {
          auto loop = new Loop(blockToNodeMap_[succ]);
          loop->SetReducibility(true);
          reducibleLoops_.push_back(loop);
          succ->SetLoop(loop);
        }
        succ->GetLoop()->AddNode(blockToNodeMap_[block]);

      } else {
        // irreducible loop
        Loop *existingLoop = nullptr;
        if (succ->GetLoop() != nullptr) {
          existingLoop = succ->GetLoop();
        } else if (block->GetLoop() != nullptr) {
          existingLoop = block->GetLoop();
        }

        if (existingLoop != nullptr) {
          // Merge into existing loop
          if (succ->GetLoop() == nullptr) {
            succ->SetLoop(existingLoop);
            existingLoop->AddNode(blockToNodeMap_[succ]);
          }
          if (block->GetLoop() == nullptr) {
            block->SetLoop(existingLoop);
            existingLoop->AddNode(blockToNodeMap_[block]);
          }
        } else {
          // New irreducible loop
          auto loop = new Loop(nullptr); // Head is null
          irreducibleLoops_.push_back(loop);
          loop->SetReducibility(false);

          loop->AddNode(blockToNodeMap_[succ]);
          loop->AddNode(blockToNodeMap_[block]);

          succ->SetLoop(loop);
          block->SetLoop(loop);
        }
      }
    } else {
      CollectBackEdgesDFSHelper(succ, markedWithGrey, markedWithBlack);
    }
  }

  markedWithGrey.erase(block);
  markedWithBlack.insert(block);
}

void LoopAnalyzer::PopulateLoops() {
  std::sort(reducibleLoops_.begin(), reducibleLoops_.end(),
            [](Loop *a, Loop *b) {
              return a->GetHead()->second > b->GetHead()->second;
            });

  for (auto *loop : reducibleLoops_) {
    ProcessLoop(loop);
  }
}

void LoopAnalyzer::ProcessLoop(Loop *loop) {
  auto headerBlock = loop->GetHead()->first;
  std::set<ir::BasicBlock *> visited;

  visited.insert(headerBlock);
  auto initialNodes = loop->GetNodes();

  for (auto *node : initialNodes) {
    if (node->first != headerBlock) {
      PopulateLoopsDFSHelper(node->first, loop, visited);
    }
  }
}

void LoopAnalyzer::PopulateLoopsDFSHelper(ir::BasicBlock *block, Loop *loop,
                                          std::set<ir::BasicBlock *> &visited) {
  if (visited.find(block) != visited.end()) {
    return;
  }
  visited.insert(block);

  auto existingLoop = block->GetLoop();

  if (existingLoop == nullptr) {
    block->SetLoop(loop);
    loop->AddNode(blockToNodeMap_[block]);
  } else if (existingLoop != loop) {
    loop->AddSubLoop(existingLoop);
  }

  for (auto pred : block->GetPreds()) {
    if (pred != nullptr) {
      PopulateLoopsDFSHelper(pred, loop, visited);
    }
  }
}

static void DumpLoopRecursively(ir::IrDumper &dumper, Loop *loop,
                                bool isReducible) {
  dumper.Add("Loop: ");
  if (isReducible && loop->GetHead() != nullptr) {
    dumper.Add("header bb ");
    dumper.Add(loop->GetHead()->first->GetId());
    dumper.Add(", ");
  }

  dumper.Add("blocks: ");
  auto &nodes = loop->GetNodes();
  size_t count = 0;
  for (auto *node : nodes) {
    if (node == nullptr || node->first == nullptr) {
      continue;
    }
    dumper.Add("bb ");
    dumper.Add(node->first->GetId());
    if (++count < nodes.size()) {
      dumper.Add(", ");
    }
  }

  if (!loop->GetSubLoops().empty()) {
    dumper.IncreaseIndent();
    dumper.Endl();
    for (auto *subLoop : loop->GetSubLoops()) {
      DumpLoopRecursively(dumper, subLoop, isReducible);
    }
    dumper.DecreaseIndent();
    return;
  }
  dumper.Endl();
}

void LoopAnalyzer::Dump(ir::IrDumper &dumper) {
  dumper.Add("Loop analysis for: ");
  dumper.Add(graph_->GetName());
  dumper.IncreaseIndent();
  dumper.Endl();

  dumper.Add("Reducible loops:");
  dumper.IncreaseIndent();
  dumper.Endl();
  if (reducibleLoops_.empty()) {
    dumper.Add("None");
    dumper.Endl();
  } else {
    for (auto *loop : reducibleLoops_) {
      DumpLoopRecursively(dumper, loop, true);
    }
  }
  dumper.DecreaseIndent();

  dumper.Endl();
  dumper.Add("Irreducible loops:");
  dumper.IncreaseIndent();
  dumper.Endl();
  if (irreducibleLoops_.empty()) {
    dumper.Add("None");
    dumper.Endl();
  } else {
    for (auto *loop : irreducibleLoops_) {
      DumpLoopRecursively(dumper, loop, false);
    }
  }
  dumper.DecreaseIndent();
  dumper.DecreaseIndent();
  dumper.Endl();
}

} // namespace analyzer