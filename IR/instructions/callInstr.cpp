#include "callInstr.h"

namespace ir::instr {

void CallInstr::Dump(IrDumper &dumper, bool dumpLiveness) {
  Instr::Dump(dumper, dumpLiveness);
  dumper.Add(" ");
  dumper.Add(methodId_);
  dumper.Add(" ");
  DumpInputs(dumper);
}

} // namespace ir::instr