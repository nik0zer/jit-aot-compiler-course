#ifndef RETURN_INSTR_H
#define RETURN_INSTR_H

#include "instr.h"

namespace ir::instr {

class ReturnInstr : public Instr {
public:
  explicit ReturnInstr(TypeId type, Instr *input)
      : Instr(InstrOpcode::RETURN, type) {
    AddInput(input);
    input->AddUser(this);
  }

  void Dump(IrDumper &dumper) override;
};

} // namespace ir::instr

#endif