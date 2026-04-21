#include "checkBoundsInstr.h"

namespace ir::instr {

void CheckBoundsInstr::Dump(IrDumper &dumper, bool dumpLiveness) {
  Instr::Dump(dumper, dumpLiveness);
  dumper.Add(" check.bounds ");
  DumpInputs(dumper);
}

} // namespace ir::instr