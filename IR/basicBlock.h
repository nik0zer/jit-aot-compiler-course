#ifndef BASICBLOCK_H
#define BASICBLOCK_H

#include "graph.h"
#include "instructions/instr.h"
#include "irDumper.h"
#include "util.h"
#include <array>
#include <cstddef>
#include <iostream>
#include <vector>
namespace ir {
constexpr std::size_t MAX_NUM_OF_SUCCESSORS = 2;

class BasicBlock {
public:
  explicit BasicBlock(MethodGraph *parent, BlockId id)
      : parent_(parent), id_(id) {}

  template <typename InstrType, typename... Args>
  instr::Instr *AllocateInstr(Args &&...args) {
    auto instr = new InstrType(std::forward<Args>(args)...);
    instr->SetInstrId(parent_->GetNextInstrId());
    if (instr->GetOpcode() == instr::InstrOpcode::PHI) {
      if (first_ == nullptr) {
        first_ = instr;
        last_ = instr;
        lastPhi_ = instr;
        return instr;
      }
      if (lastPhi_ == nullptr) {
        first_->SetPrevInstr(instr);
        instr->SetNextInstr(first_);
        firstNonPhi_ = first_;
        first_ = instr;
        lastPhi_ = instr;
        return instr;
      }
      lastPhi_->SetNextInstr(instr);
      instr->SetPrevInstr(lastPhi_);
      lastPhi_ = instr;
      if (firstNonPhi_ != nullptr) {
        instr->SetNextInstr(firstNonPhi_);
        firstNonPhi_->SetPrevInstr(instr);
        return instr;
      }
      last_ = instr;
      return instr;
    }
    if (first_ == nullptr) {
      first_ = instr;
      firstNonPhi_ = instr;
      last_ = instr;
      return instr;
    }
    last_->SetNextInstr(instr);
    instr->SetPrevInstr(last_);
    last_ = instr;
    return instr;
  }

  template <typename InstrType, typename... Args>
  instr::Instr *AllocateInstrAt(size_t index, Args &&...args) {
    if (first_ == nullptr) {
      return nullptr;
    }

    auto insertInstr = first_;
    for (size_t i = 0; i < index; i++) {
      insertInstr = insertInstr->GetNextInstr();
      if (insertInstr == nullptr) {
        return nullptr;
      }
    }

    return AllocateInstrAfter<InstrType>(insertInstr,
                                         std::forward<Args>(args)...);
  }

  template <typename InstrType, typename... Args>
  instr::Instr *AllocateInstrAfter(instr::Instr *insertInstr, Args &&...args) {
    auto instr = new InstrType(std::forward<Args>(args)...);
    instr->SetInstrId(parent_->GetNextInstrId());

    auto nextInstr = insertInstr->GetNextInstr();
    insertInstr->SetNextInstr(instr);
    instr->SetPrevInstr(insertInstr);
    instr->SetNextInstr(nextInstr);
    if (nextInstr != nullptr) {
      nextInstr->SetPrevInstr(instr);
    }

    return instr;
  }

  void SetParent(MethodGraph *parent) { parent_ = parent; }

  void SetPreds(const std::vector<BasicBlock *> &preds) { preds_ = preds; }
  void SetPreds(std::vector<BasicBlock *> &&preds) {
    preds_ = std::move(preds);
  }
  void SetSuccs(const std::array<BasicBlock *, MAX_NUM_OF_SUCCESSORS> &succs) {
    succs_ = succs;
  }
  void SetSuccs(std::array<BasicBlock *, MAX_NUM_OF_SUCCESSORS> &&succs) {
    succs_ = std::move(succs);
  }

  const std::vector<BasicBlock *> &GetPreds() const { return preds_; }
  const std::array<BasicBlock *, MAX_NUM_OF_SUCCESSORS> &GetSuccs() const {
    return succs_;
  }

  const instr::Instr *GetFirstInstr() const { return first_; }
  const instr::Instr *GetLastPhiInstr() const { return lastPhi_; }
  const instr::Instr *GetFirstNonPhiInstr() const { return firstNonPhi_; }
  const instr::Instr *GetLastInstr() const { return last_; }

  void Dump(IrDumper &dumper);

  void SetId(BlockId id) { id_ = id; }
  BlockId GetId() const { return id_; }

  ~BasicBlock() {
    auto currentInstr = first_;
    while (currentInstr != nullptr) {
      auto nextInstr = currentInstr->GetNextInstr();
      delete currentInstr;
      currentInstr = nextInstr;
    }
  }

  BasicBlock *GetIDominator() { return iDominator_; }
  void SetIDominator(BasicBlock *iDominator) { iDominator_ = iDominator; }

  const std::vector<Analyzer::BlockWithIndex> &GetDominators() const {
    return dominators_;
  }
  void SetDominators(const std::vector<Analyzer::BlockWithIndex> &dominators) {
    dominators_ = dominators;
  }

  const std::vector<Analyzer::BlockWithIndex> &GetDominatedBlocks() const {
    return dominatedBlocks_;
  }
  void SetDominatedBlocks(
      const std::vector<Analyzer::BlockWithIndex> &dominatedBlocks) {
    dominatedBlocks_ = dominatedBlocks;
  }

  friend class MethodGraph;
  friend class Analyzer::DominatorAnalyzer;

private:
  void DumpPredecessors(IrDumper &dumper);
  void DumpSuccessors(IrDumper &dumper);

  MethodGraph *parent_;
  BlockId id_{0};
  instr::Instr *first_{nullptr};
  instr::Instr *lastPhi_{nullptr};
  instr::Instr *firstNonPhi_{nullptr};
  instr::Instr *last_{nullptr};
  std::vector<BasicBlock *> preds_{};
  std::array<BasicBlock *, MAX_NUM_OF_SUCCESSORS> succs_{};

  std::vector<Analyzer::BlockWithIndex> dominators_{};
  std::vector<Analyzer::BlockWithIndex> dominatedBlocks_{};
  BasicBlock *iDominator_{nullptr};
};

} // namespace ir

#endif