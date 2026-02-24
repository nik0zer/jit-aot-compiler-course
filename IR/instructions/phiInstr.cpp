#include "phiInstr.h"

namespace ir::instr {

void PhiInstr::Dump(IrDumper &dumper, bool dumpLiveness) {
  Instr::Dump(dumper, dumpLiveness);
  dumper.Add(" phi ");
  DumpInputs(dumper);
}

} // namespace ir::instr