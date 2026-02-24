#ifndef IF_INSTR_H
#define IF_INSTR_H

#include "instr.h"

namespace ir::instr {
enum class IfType { EQ, NE, LT, LE, GT, GE };

class IfInstr : public Instr {
public:
  explicit IfInstr(Instr *lhs, Instr *rhs, IfType type)
      : Instr(InstrOpcode::IF, TypeId::VOID, {lhs, rhs}, {}), type_(type) {}

  bool IsControllFlow() override final { return true; }

  void Dump(IrDumper &dumper, bool dumpLiveness = false) override;

private:
  IfType type_;
};

} // namespace ir::instr

#endif