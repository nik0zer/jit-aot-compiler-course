#ifndef JUMP_INSTR_H
#define JUMP_INSTR_H

#include "instr.h"

namespace ir::instr {

class JumpInstr : public Instr {
public:
    explicit JumpInstr() : Instr(InstrOpcode::JUMP) {}

    bool IsControllFlow() override final { return true; }
};

} // namespace ir::instr

#endif