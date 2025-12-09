#ifndef LOOP_ANALYZER_H
#define LOOP_ANALYZER_H

#include "dominatorAnalyzer.h"
#include "graph.h"
#include "util.h"
#include <unordered_map>

namespace analyzer {

class Loop {
public:
  Loop(BlockWithIndex *head) { this->head = head; }

  BlockWithIndex *GetHead() { return head; }

  void AddNode(BlockWithIndex *node) { nodes.insert(node); }

  std::set<BlockWithIndex *> &GetNodes() { return nodes; }

  std::set<Loop *> &GetSubLoops() { return subLoops; }

  void AddSubLoop(Loop *loop) { subLoops.insert(loop); }

private:
  BlockWithIndex *head;
  std::set<BlockWithIndex *> nodes;
  std::set<Loop *> subLoops;
};

class LoopAnalyzer {
public:
  LoopAnalyzer(ir::MethodGraph &graph,
               analyzer::DominatorAnalyzer &dominatorAnalyzer)
      : graph_(&graph), dominatorAnalyzer_(dominatorAnalyzer) {}

  ~LoopAnalyzer();

  void FindLoops(ir::BasicBlock *startBlock = nullptr,
                 bool dominatorsFound = false);

  void Dump(ir::IrDumper &dumper);

private:
  void CollectBackEdges(ir::BasicBlock *startBlock);
  void CollectBackEdgesDFSHelper(ir::BasicBlock *block,
                                 std::set<ir::BasicBlock *> &markedWithGrey,
                                 std::set<ir::BasicBlock *> &markedWithBlack);

  void PopulateLoops();
  void ProcessLoop(Loop *loop);
  void PopulateLoopsDFSHelper(ir::BasicBlock *block, Loop *loop,
                              std::set<ir::BasicBlock *> &visited);

  std::vector<Loop *> irreducibleLoops_;
  std::vector<Loop *> reducibleLoops_;

  ir::MethodGraph *graph_;
  analyzer::DominatorAnalyzer &dominatorAnalyzer_;

  std::vector<BlockWithIndex> cachedRpo_;
  std::unordered_map<ir::BasicBlock *, BlockWithIndex *> blockToNodeMap_;
};

} // namespace analyzer

#endif