#ifndef GRAPH_H
#define GRAPH_H

#include "instructions/instr.h"
#include "macro.h"
#include <cstddef>
#include <limits>
#include <string>

namespace analyzer {
class DominatorAnalyzer;
class DominatorTree;
class LinearOrderAnalyzer;
} // namespace analyzer

namespace ir {
using MethodId = std::size_t;
using ParamId = std::size_t;
using BlockId = std::size_t;

class MethodGraph {
public:
  explicit MethodGraph(const std::string &&name, MethodId id)
      : name_(name), id_(id) {}

  explicit MethodGraph(MethodGraph &&other) = delete;
  explicit MethodGraph(MethodGraph &other) = delete;

  void AddBlock(BasicBlock *block);
  bool RemoveBlock(BasicBlock *block);
  BasicBlock *AllocateBlock();

  const std::vector<BasicBlock *> &GetBlocks() const { return blocks_; }
  void SetBlocks(const std::vector<BasicBlock *> &&blocks) { blocks_ = blocks; }

  const std::vector<BasicBlock *> &GetLinearOrderedBlocks() const {
    return linearOrderedBlocks_;
  }
  void SetLinearOrderedBlocks(const std::vector<BasicBlock *> &&blocks) {
    linearOrderedBlocks_ = blocks;
  }

  const std::string &GetName() const { return name_; }

  instr::InstrId GetNextInstrId() {
    auto freeId = freeId_++;
    if (freeId == std::numeric_limits<instr::InstrId>::max()) {
      UNREACHABLE();
    }
    return freeId;
  }
  instr::InstrId GetNextBlockId() {
    auto freeId = freeBlockId_++;
    if (freeId == std::numeric_limits<instr::InstrId>::max()) {
      UNREACHABLE();
    }
    return freeId;
  }

  void Dump(IrDumper &dumper);

  ~MethodGraph();

  friend class analyzer::DominatorAnalyzer;
  friend class analyzer::DominatorTree;
  friend class analyzer::LinearOrderAnalyzer;

private:
  std::vector<BasicBlock *> blocks_;
  std::vector<BasicBlock *> linearOrderedBlocks_;
  std::string name_;
  MethodId id_;
  instr::InstrId freeId_{0};
  BlockId freeBlockId_{0};
};

} // namespace ir

#endif