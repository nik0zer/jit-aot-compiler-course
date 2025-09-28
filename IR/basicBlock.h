#ifndef BASICBLOCK_H
#define BASICBLOCK_H

#include "graph.h"
#include "instructions/instr.h"
#include <iostream>
#include <vector>
namespace ir {

class BasicBlock {
public:
  explicit BasicBlock(MethodGraph *parent) : parent_(parent) {}

  template <typename InstrType, typename... Args>
  instr::Instr *AllocateInstr(Args &&...args) {
    auto instr = new InstrType(std::forward<Args>(args)...);
    instr->SetInstrId(parent_->GetNextInstrId());
    if (first_ == nullptr) {
      first_ = instr;
      last_ = instr;
      return instr;
    }
    last_->SetNextInstr(instr);
    instr->SetPrevInstr(last_);
    last_ = instr;
    return instr;
  }

  const instr::Instr *GetFirstInstr() const { return first_; }
  const instr::Instr *GetLastInstr() const { return last_; }

  ~BasicBlock() {
    auto currentInstr = first_;
    while (currentInstr != nullptr) {
      auto nextInstr = currentInstr->GetNextInstr();
      delete currentInstr;
      currentInstr = nextInstr;
    }
  }

private:
  MethodGraph *parent_;
  instr::Instr *first_ = nullptr;
  instr::Instr *last_ = nullptr;
};

} // namespace ir

#endif