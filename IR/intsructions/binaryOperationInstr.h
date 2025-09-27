#ifndef BINARY_OP_INSTR_H
#define BINARY_OP_INSTR_H

#include "instr.h"

namespace ir::instr {
class BinaryOperationInstr : public Instr {
public:
  BinaryOperationInstr(TypeId type) : Instr(InstrOpcode::BINARY_OPERATION, type) {}
  BinaryOperationInstr(Instr *lhs, Instr *rhs, TypeId type)
      : Instr(InstrOpcode::BINARY_OPERATION, type) {
    AddInput(lhs);
    AddInput(rhs);
    lhs->AddUser(this);
    rhs->AddUser(this);
  }
};
} // namespace ir::instr

#endif