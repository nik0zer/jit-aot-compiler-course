#ifndef CAST_INSTR_H
#define CAST_INSTR_H

#include "instr.h"

namespace ir::instr {

class CastInstr : public Instr {
public:
    explicit CastInstr() : Instr(InstrOpcode::CAST) {}
};

} // namespace ir::instr

#endif