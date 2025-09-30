#ifndef PHI_INSTR_H
#define PHI_INSTR_H

#include "instr.h"

namespace ir::instr {

class PhiInstr : public Instr {
public:
  explicit PhiInstr(TypeId type, const std::vector<Instr *> &&inputs)
      : Instr(InstrOpcode::PHI, type, std::move(inputs), {}) {}
  explicit PhiInstr(TypeId type, const std::vector<Instr *> &inputs)
      : Instr(InstrOpcode::PHI, type, std::move(inputs), {}) {}

  void Dump(IrDumper &dumper) override;
};

} // namespace ir::instr

#endif