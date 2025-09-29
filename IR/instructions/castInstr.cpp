#include "castInstr.h"
#include "instructions/instr.h"

namespace ir::instr {

void CastInstr::Dump(IrDumper &dumper) {
    Instr::Dump(dumper);
    dumper.Add(" ");
    dumper.Add("cast");
    dumper.Add(" ");
    DumpInputs(dumper);
}

} // namespace ir::instr