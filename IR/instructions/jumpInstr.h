#ifndef JUMP_INSTR_H
#define JUMP_INSTR_H

#include "graph.h"
#include "instr.h"
#include <cstddef>

namespace ir::instr {

class JumpInstr : public Instr {
public:
  explicit JumpInstr(BasicBlock *target)
      : Instr(InstrOpcode::JUMP, TypeId::U32), target_(target) {}

  bool IsControllFlow() override final { return true; }

private:
  BasicBlock *target_;
};

} // namespace ir::instr

#endif