#include "checkNullInstr.h"

namespace ir::instr {

void CheckNullInstr::Dump(IrDumper &dumper, bool dumpLiveness) {
  Instr::Dump(dumper, dumpLiveness);
  dumper.Add(" check.null ");
  DumpInputs(dumper);
}

} // namespace ir::instr