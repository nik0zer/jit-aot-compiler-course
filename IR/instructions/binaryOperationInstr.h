#ifndef BINARY_OP_INSTR_H
#define BINARY_OP_INSTR_H

#include "instr.h"

namespace ir::instr {
enum class BinaryOperation { ADD, SUB, MUL, DIV, AND, OR };

class BinaryOperationInstr : public Instr {
public:
  BinaryOperationInstr(const BinaryOperation &op, Instr *lhs, Instr *rhs,
                       TypeId type)
      : Instr(InstrOpcode::BINARY_OPERATION, type) {
    AddInput(lhs);
    AddInput(rhs);
    lhs->AddUser(this);
    rhs->AddUser(this);
  }
};
} // namespace ir::instr

#endif