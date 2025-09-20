#ifndef BINARY_OP_INSTR_H
#define BINARY_OP_INSTR_H

#include "instr.h"

namespace ir::instr {
class BinaryOperationInstr : public Instr {
public:
    BinaryOperationInstr() : Instr(InstrOpcode::BINARY_OPERATION) {}
};
}   // namespace ir::instr

#endif