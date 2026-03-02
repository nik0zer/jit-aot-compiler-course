#ifndef GRAPH_H
#define GRAPH_H

#include "instructions/instr.h"
#include "macro.h"
#include "regAlloc.h"
#include <cstddef>
#include <limits>
#include <string>
#include <vector>

namespace analyzer {
class DominatorAnalyzer;
class DominatorTree;
class LinearOrderAnalyzer;
} // namespace analyzer

namespace ir {
using MethodId = std::size_t;
using ParamId = std::size_t;
using BlockId = std::size_t;

class MemPlaceInfo {
public:
  MemPlaceInfo(bool isReg, uint8_t maxSize, size_t id) : isReg_(isReg), maxSize_(maxSize), id_(id) {}

  bool IsReg() const { return isReg_; }
  uint8_t GetMaxSize() const { return maxSize_; }
  size_t GetId() const { return id_; }

  void SetIsReg(bool isReg) { isReg_ = isReg; }
  void SetMaxSize(uint8_t maxSize) { maxSize_ = maxSize; }
  void SetId(size_t id) { id_ = id; }

private:
  bool isReg_ {true};
  uint8_t maxSize_ {64};
  size_t id_;
};

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

  void SetRegistersInfo(std::vector<MemPlaceInfo> &&info) { registers_ = info; }
  const std::vector<MemPlaceInfo> &GetRegistersInfo() const { return registers_; }

  const std::vector<MemPlaceInfo> &GetStackSlotsInfo() const { return stackSlots_; }

  void Dump(IrDumper &dumper, bool dumpLiveness = false) const;

  ~MethodGraph();

  friend class analyzer::DominatorAnalyzer;
  friend class analyzer::DominatorTree;
  friend class analyzer::LinearOrderAnalyzer;
  friend class analyzer::RegAlloc;

private:
  std::vector<MemPlaceInfo> registers_;
  std::vector<MemPlaceInfo> stackSlots_;
  std::vector<BasicBlock *> blocks_;
  std::vector<BasicBlock *> linearOrderedBlocks_;
  std::string name_;
  MethodId id_;
  instr::InstrId freeId_{0};
  BlockId freeBlockId_{0};
};

} // namespace ir

#endif