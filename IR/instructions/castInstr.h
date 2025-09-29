#ifndef CAST_INSTR_H
#define CAST_INSTR_H

#include "instr.h"

namespace ir::instr {

class CastInstr : public Instr {
public:
  explicit CastInstr(TypeId type, Instr *input)
      : Instr(InstrOpcode::CAST, type) {
    AddInput(input);
  }

  void Dump(IrDumper &dumper) override;
};

} // namespace ir::instr

#endif