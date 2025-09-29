#include "callInstr.h"

namespace ir::instr {

void CallInstr::Dump(IrDumper &dumper) {
    Instr::Dump(dumper);
    dumper.Add(" ");
    dumper.Add(methodId_);
    dumper.Add(" ");
    for (auto input : inputs_) {
        DumpInput(dumper, input);
        dumper.Add(" ");
    }
}

} // namespace ir::instr