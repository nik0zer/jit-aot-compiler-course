#include "storeArrayInstr.h"

namespace ir::instr {

void StoreArrayInstr::Dump(IrDumper &dumper, bool dumpLiveness) {
  Instr::Dump(dumper, dumpLiveness);
  dumper.Add(" starr ");
  DumpInputs(dumper);
}

} // namespace ir::instr