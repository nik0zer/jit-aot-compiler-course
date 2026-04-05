#include "callStaticInstr.h"

namespace ir::instr {

void CallStaticInstr::Dump(IrDumper &dumper, bool dumpLiveness) {
  Instr::Dump(dumper, dumpLiveness);
  dumper.Add(" call.static ");
  dumper.Add(methodName_);
  dumper.Add(" ");
  DumpInputs(dumper);
}

} // namespace ir::instr