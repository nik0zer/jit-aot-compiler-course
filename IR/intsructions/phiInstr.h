#ifndef PHI_INSTR_H
#define PHI_INSTR_H

#include "instr.h"

namespace ir::instr {

class PhiInstr : public Instr {
public:
  explicit PhiInstr() : Instr(InstrOpcode::PHI) {}
};

} // namespace ir::instr

#endif