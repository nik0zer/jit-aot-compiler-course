#ifndef BINARY_OP_INSTR_H
#define BINARY_OP_INSTR_H

#include "instr.h"
#include "irDumper.h"

namespace ir::instr {
enum class BinaryOperationType { ADD, SUB, MUL, DIV, AND, OR };

class BinaryOperationInstr : public Instr {
public:
  BinaryOperationInstr(TypeId type, const BinaryOperationType &op, Instr *lhs,
                       Instr *rhs)
      : Instr(InstrOpcode::BINARY_OPERATION, type), op_(op) {
    AddInput(lhs);
    AddInput(rhs);
    lhs->AddUser(this);
    rhs->AddUser(this);
  }

  void SetOperation(BinaryOperationType op) { op_ = op; }
  BinaryOperationType GetOperation() const { return op_; }

  void Dump(IrDumper &dumper) override;

private:
  BinaryOperationType op_;
};
} // namespace ir::instr

#endif