#include "castInstr.h"
#include "instructions/instr.h"

namespace ir::instr {

void CastInstr::Dump(IrDumper &dumper, bool dumpLiveness) {
  Instr::Dump(dumper, dumpLiveness);
  dumper.Add(" cast ");
  DumpInputs(dumper);
}

} // namespace ir::instr