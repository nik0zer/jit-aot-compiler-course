#ifndef CHECK_NULL_INSTR_H
#define CHECK_NULL_INSTR_H

#include "instr.h"

namespace ir::instr {
class CheckNullInstr : public Instr {
public:
  CheckNullInstr(Instr *inputValue)
      : Instr(InstrOpcode::CHECK_NULL, TypeId::VOID) {
    AddInput(inputValue);
  }

  bool IsControllFlow() override { return true; }

  void Dump(IrDumper &dumper, bool dumpLiveness = false) override;
};
} // namespace ir::instr

#endif