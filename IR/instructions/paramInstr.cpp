#include "paramInstr.h"
#include "instructions/instr.h"

namespace ir::instr {

void ParamInstr::Dump(IrDumper &dumper, bool dumpLiveness) {
  Instr::Dump(dumper, dumpLiveness);
  dumper.Add(" param ");
  dumper.Add(id_);
}

} // namespace ir::instr