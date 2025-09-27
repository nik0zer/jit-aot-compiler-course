#ifndef CALL_INSTR_H
#define CALL_INSTR_H

#include "instr.h"

namespace ir::instr {

class CallInstr : public Instr {
public:
  explicit CallInstr() : Instr(InstrOpcode::CALL) {}

  bool IsControllFlow() override final { return true; }
};

} // namespace ir::instr

#endif