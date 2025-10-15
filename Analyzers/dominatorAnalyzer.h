#ifndef DOMINATOR_ANALYZER
#define DOMINATOR_ANALYZER

#include "graph.h"
#include "util.h"

namespace Analyzer {
class DominatorAnalyzer {
public:
  explicit DominatorAnalyzer(const ir::MethodGraph &graph) : graph_(graph) {}

  std::vector<BlockWithIndex> RPO(ir::BasicBlock *startBlock = nullptr,
                                  ir::BasicBlock *ignoredBlock = nullptr);
  std::vector<ir::BasicBlock *> DFSPO(ir::BasicBlock *startBlock = nullptr,
                                      ir::BasicBlock *ignoredBlock = nullptr);
  void DumpRPO(ir::IrDumper &dumper, ir::BasicBlock *startBlock = nullptr,
               ir::BasicBlock *ignoredBlock = nullptr);
  void FindDominators(ir::BasicBlock *startBlock = nullptr);
  void DumpDominators(ir::IrDumper &dumper,
                      ir::BasicBlock *startBlock = nullptr);

private:
  const ir::MethodGraph &graph_;
};

class DominatorTree {
public:
  explicit DominatorTree(const ir::MethodGraph &graph,
                         ir::BasicBlock *startBlock);

  struct TreeNode {
    BlockWithIndex block;
    std::vector<TreeNode *> children;
    TreeNode *parent;

    TreeNode(BlockWithIndex block, TreeNode *parent = nullptr)
        : block(block), parent(parent) {}
  };

  void Dump(ir::IrDumper &dumper);

  ~DominatorTree() { destroyNodes(root_); }

private:
  void destroyNodes(TreeNode *node) {
    if (node == nullptr)
      return;
    for (auto child : node->children) {
      if (child != nullptr) {
        destroyNodes(child);
        delete child;
      }
    }
  }

  TreeNode *createNode(BlockWithIndex block, TreeNode *parent);

  TreeNode *root_;
  const ir::MethodGraph &graph_;
};

} // namespace Analyzer

#endif