#ifndef PARAM_INSTR_H
#define PARAM_INSTR_H

#include "instr.h"

namespace ir::instr {

class ParamInstr : public Instr {
public:
  explicit ParamInstr(TypeId type) : Instr(InstrOpcode::PARAM, type) {}
};

} // namespace ir::instr

#endif