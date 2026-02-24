#include "linearOrderAnalyzer.h"
#include "irDumper.h"
#include <algorithm>
#include <set>
#include <unordered_map>
#include <vector>

namespace analyzer {

namespace {

bool IsLoopHeader(ir::BasicBlock *bb) {
  auto loop = bb->GetLoop();
  if (loop == nullptr) {
    return false;
  }

  auto headNode = loop->GetHead();
  if (headNode == nullptr) {
    return false;
  }

  return headNode->first == bb;
}

bool IsBackEdge(ir::BasicBlock *pred, ir::BasicBlock *succ) {
  if (!IsLoopHeader(succ)) {
    return false;
  }

  auto succLoop = succ->GetLoop();
  auto predLoop = pred->GetLoop();

  if (predLoop == nullptr) {
    return false;
  }

  if (predLoop == succLoop) {
    return true;
  }

  const auto &subLoops = succLoop->GetSubLoops();
  if (subLoops.find(predLoop) != subLoops.end()) {
    return true;
  }

  return false;
}

size_t CalculateWaitCount(ir::BasicBlock *bb) {
  size_t count = 0;
  bool isHeader = IsLoopHeader(bb);

  for (auto *pred : bb->GetPreds()) {
    if (pred == nullptr || isHeader && IsBackEdge(pred, bb)) {
      continue;
    }

    count++;
  }
  return count;
}

ir::BasicBlock *PickNextBlock(std::vector<ir::BasicBlock *> &readyList,
                              ir::BasicBlock *lastScheduled) {
  if (readyList.empty()) {
    return nullptr;
  }

  auto bestCandidateIt = readyList.end();

  if (lastScheduled != nullptr) {
    for (auto *succ : lastScheduled->GetSuccs()) {
      if (succ == nullptr)
        continue;

      auto it = std::find(readyList.begin(), readyList.end(), succ);
      if (it != readyList.end()) {
        bestCandidateIt = it;
        break;
      }
    }
  }

  if (bestCandidateIt == readyList.end() && lastScheduled != nullptr) {
    auto lastLoop = lastScheduled->GetLoop();
    if (lastLoop != nullptr) {
      for (auto it = readyList.begin(); it != readyList.end(); ++it) {
        if ((*it)->GetLoop() == lastLoop) {
          bestCandidateIt = it;
          break;
        }
      }
    }
  }

  if (bestCandidateIt == readyList.end()) {
    bestCandidateIt = readyList.begin();
  }

  ir::BasicBlock *selected = *bestCandidateIt;
  readyList.erase(bestCandidateIt);

  return selected;
}

}

void LinearOrderAnalyzer::CreateLinearOrder(ir::BasicBlock *startBlock) {
  auto &blocks = graph_.GetBlocks();
  if (blocks.empty()) {
    return;
  }

  if (startBlock == nullptr) {
    startBlock = blocks.front();
  }

  graph_.linearOrderedBlocks_.clear();
  graph_.linearOrderedBlocks_.reserve(blocks.size());

  std::unordered_map<ir::BasicBlock *, size_t> waitCounts;
  for (auto *bb : blocks) {
    waitCounts[bb] = CalculateWaitCount(bb);
  }

  std::vector<ir::BasicBlock *> readyList;
  std::set<ir::BasicBlock *> visited;

  readyList.push_back(startBlock);
  visited.insert(startBlock);

  ir::BasicBlock *lastScheduled = nullptr;

  auto fallbackIt = blocks.begin();

  while (!readyList.empty()) {
    ir::BasicBlock *current = PickNextBlock(readyList, lastScheduled);

    graph_.linearOrderedBlocks_.push_back(current);
    lastScheduled = current;

    for (auto *succ : current->GetSuccs()) {
      if (succ == nullptr)
        continue;
      if (visited.count(succ))
        continue;

      auto it = waitCounts.find(succ);
      if (it != waitCounts.end()) {
        if (it->second > 0) {
          it->second--;
        }

        if (it->second == 0) {
          readyList.push_back(succ);
          visited.insert(succ);
        }
      }
    }

    if (readyList.empty() &&
        (graph_.linearOrderedBlocks_.size() < blocks.size())) {
      while (fallbackIt != blocks.end() &&
             visited.find(*fallbackIt) != visited.end()) {
        ++fallbackIt;
      }

      if (fallbackIt != blocks.end()) {
        ir::BasicBlock *forceBlock = *fallbackIt;
        readyList.push_back(forceBlock);
        visited.insert(forceBlock);
      } else {
        break;
      }
    }
  }

  if (graph_.linearOrderedBlocks_.size() < blocks.size()) {
    for (auto *bb : blocks) {
      if (visited.find(bb) == visited.end()) {
        graph_.linearOrderedBlocks_.push_back(bb);
      }
    }
  }
}

void LinearOrderAnalyzer::Dump(ir::IrDumper &dumper) {
  dumper.Add("linear order: ");
  for (auto *bb : graph_.linearOrderedBlocks_) {
    dumper.Add("bb ");
    dumper.Add(bb->GetId());
    if (bb != graph_.linearOrderedBlocks_.back()) {
      dumper.Add(", ");
    }
  }
  dumper.Endl();
}

} // namespace analyzer