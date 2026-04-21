#include "loadArrayInstr.h"

namespace ir::instr {

void LoadArrayInstr::Dump(IrDumper &dumper, bool dumpLiveness) {
  Instr::Dump(dumper, dumpLiveness);
  dumper.Add(" ldarr ");
  DumpInputs(dumper);
}

} // namespace ir::instr