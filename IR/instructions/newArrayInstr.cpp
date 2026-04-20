#include "newArrayInstr.h"


namespace ir::instr {

void NewArrayInstr::Dump(IrDumper &dumper, bool dumpLiveness) {
    Instr::Dump(dumper, dumpLiveness);
    dumper.Add(" newarr.");
    dumper.Add(std::string(TypeIdToString(elementType_)));
    dumper.Add(" ");
    DumpInputs(dumper);
}

}