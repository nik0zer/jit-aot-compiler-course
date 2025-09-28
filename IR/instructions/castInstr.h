#ifndef CAST_INSTR_H
#define CAST_INSTR_H

#include "instr.h"

namespace ir::instr {

class CastInstr : public Instr {
public:
  explicit CastInstr(TypeId type) : Instr(InstrOpcode::CAST, type) {}

  explicit CastInstr(Instr *input, TypeId type)
      : Instr(InstrOpcode::CAST, type) {
    AddInput(input);
  }
};

} // namespace ir::instr

#endif