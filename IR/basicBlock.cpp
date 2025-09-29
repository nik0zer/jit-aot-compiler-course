#include "basicBlock.h"

namespace ir {

void BasicBlock::Dump(IrDumper &dumper) {
    dumper.Add("basic block: ");
    dumper.Add(id_);
    dumper.Endl();
    dumper.IncreaseIndent();
    auto instr = first_;
    while (instr != nullptr) {
        instr->Dump(dumper);
        instr = instr->GetNextInstr();
        dumper.Endl();
    }
    dumper.DecreaseIndent();
}

} // namespace ir