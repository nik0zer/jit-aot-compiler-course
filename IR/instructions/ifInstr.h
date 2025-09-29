#ifndef IF_INSTR_H
#define IF_INSTR_H

#include "instr.h"

namespace ir::instr {
enum class IfType { EQ, NE, LT, LE, GT, GE };

class IfInstr : public Instr {
public:
  explicit IfInstr(BasicBlock *target1, BasicBlock *target2, Instr *lhs, Instr *rhs, IfType type)
      : Instr(InstrOpcode::IF, TypeId::U32, {lhs, rhs}, {}), target1_(target1), target2_(target2), type_(type) {}

  bool IsControllFlow() override final { return true; }

  void Dump(IrDumper &dumper) override;

private:
  BasicBlock *target1_;
  BasicBlock *target2_;
  IfType type_;
};

}

#endif