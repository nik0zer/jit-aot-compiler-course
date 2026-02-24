#ifndef BINARY_OP_INSTR_H
#define BINARY_OP_INSTR_H

#include "instr.h"
#include "irDumper.h"

namespace ir::instr {
enum class BinaryOperationType { ADD, SUB, MUL, DIV, AND, OR, ASHR };

class BinaryOperationInstr : public Instr {
public:
  BinaryOperationInstr(TypeId type, const BinaryOperationType &op, Instr *lhs,
                       Instr *rhs)
      : Instr(InstrOpcode::BINARY_OPERATION, type), op_(op) {
    AddInput(lhs);
    AddInput(rhs);
    if (lhs != nullptr) {
      lhs->AddUser(this);
    }
    if (rhs != nullptr) {
      rhs->AddUser(this);
    }
  }

  void SetOperation(BinaryOperationType op) { op_ = op; }
  BinaryOperationType GetOperation() const { return op_; }

  void Dump(IrDumper &dumper, bool dumpLiveness = false) override;

private:
  BinaryOperationType op_;
};
} // namespace ir::instr

#endif