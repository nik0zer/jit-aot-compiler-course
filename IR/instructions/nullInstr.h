#ifndef NULL_INSTR_H
#define NULL_INSTR_H

#include "instr.h"

namespace ir::instr {
class NullInstr : public Instr {
public:
    NullInstr() : Instr(InstrOpcode::NULL_INSTR, TypeId::REF) {}

    void Dump(IrDumper &dumper, bool dumpLiveness = false) override;
};
}

#endif