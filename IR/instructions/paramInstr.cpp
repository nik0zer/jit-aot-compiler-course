#include "paramInstr.h"
#include "instructions/instr.h"

namespace ir::instr {

void ParamInstr::Dump(IrDumper &dumper) {
  Instr::Dump(dumper);
  dumper.Add(" param ");
  dumper.Add(id_);
}

} // namespace ir::instr