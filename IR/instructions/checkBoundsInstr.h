#ifndef CHECK_BOUNDS_INSTR_H
#define CHECK_BOUNDS_INSTR_H

#include "instr.h"

namespace ir::instr {
class CheckBoundsInstr : public Instr {
public:
  CheckBoundsInstr(Instr *array, Instr *index)
      : Instr(InstrOpcode::CHECK_BOUNDS, TypeId::VOID) {
    AddInput(array);
    AddInput(index);
  }

  bool IsControllFlow() override { return true; }

  void Dump(IrDumper &dumper, bool dumpLiveness = false) override;
};
} // namespace ir::instr

#endif