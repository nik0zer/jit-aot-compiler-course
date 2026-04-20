#include "newStringInstr.h"

namespace ir::instr {

void NewStringInstr::Dump(IrDumper &dumper, bool dumpLiveness)
{
    Instr::Dump(dumper, dumpLiveness);
    dumper.Add(" newstr \"");
    dumper.Add(value_);
    dumper.Add("\"");
}

}