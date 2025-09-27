#ifndef BINARY_OP_INSTR_H
#define BINARY_OP_INSTR_H

#include "instr.h"

namespace ir::instr {
class BinaryOperationInstr : public Instr {
public:
  BinaryOperationInstr() : Instr(InstrOpcode::BINARY_OPERATION) {}
  BinaryOperationInstr(Instr *lhs, Instr *rhs)
      : Instr(InstrOpcode::BINARY_OPERATION) {
    AddInput(lhs);
    AddInput(rhs);
    lhs->AddUser(this);
    rhs->AddUser(this);
  }
};
} // namespace ir::instr

#endif