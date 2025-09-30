#include "returnInstr.h"
#include "instructions/instr.h"

namespace ir::instr {

void ReturnInstr::Dump(IrDumper &dumper) {
  Instr::Dump(dumper);
  dumper.Add(" return ");
  DumpInputs(dumper);
}

} // namespace ir::instr