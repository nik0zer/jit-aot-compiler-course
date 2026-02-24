#include "returnInstr.h"
#include "instructions/instr.h"

namespace ir::instr {

void ReturnInstr::Dump(IrDumper &dumper, bool dumpLiveness) {
  Instr::Dump(dumper, dumpLiveness);
  dumper.Add(" return ");
  DumpInputs(dumper);
}

} // namespace ir::instr