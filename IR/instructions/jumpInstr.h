#ifndef JUMP_INSTR_H
#define JUMP_INSTR_H

#include "graph.h"
#include "instr.h"
#include <cstddef>

namespace ir::instr {

class JumpInstr : public Instr {
public:
  explicit JumpInstr(TypeId type, LabelId target)
      : Instr(InstrOpcode::JUMP, type), target_(target) {}

  bool IsControllFlow() override final { return true; }

private:
  LabelId target_;
};

} // namespace ir::instr

#endif