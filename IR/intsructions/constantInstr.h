#ifndef CONSTANT_INSTR_H
#define CONSTANT_INSTR_H

#include "instr.h"

namespace ir::instr {

class ConstantInstr : public Instr {
public:
    explicit ConstantInstr() : Instr(InstrOpcode::CONSTANT) {}
};

} // namespace ir::instr

#endif