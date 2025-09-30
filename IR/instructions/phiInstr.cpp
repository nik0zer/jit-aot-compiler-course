#include "phiInstr.h"

namespace ir::instr {

void PhiInstr::Dump(IrDumper &dumper) {
  Instr::Dump(dumper);
  dumper.Add(" phi ");
  DumpInputs(dumper);
}

} // namespace ir::instr