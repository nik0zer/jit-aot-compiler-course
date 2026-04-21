#include "nullInstr.h"

namespace ir::instr {

void NullInstr::Dump(IrDumper &dumper, bool dumpLiveness) {
    Instr::Dump(dumper, dumpLiveness);
    dumper.Add(" null");
}

}