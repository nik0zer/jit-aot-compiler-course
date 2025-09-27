#ifndef RETURN_INSTR_H
#define RETURN_INSTR_H

#include "instr.h"

namespace ir::instr {

class ReturnInstr : public Instr {
public:
  explicit ReturnInstr() : Instr(InstrOpcode::RETURN) {}
};

} // namespace ir::instr

#endif