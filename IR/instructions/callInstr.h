#ifndef CALL_INSTR_H
#define CALL_INSTR_H

#include "graph.h"
#include "instr.h"
#include <vector>

namespace ir::instr {

class CallInstr : public Instr {
public:
  explicit CallInstr(TypeId type) : Instr(InstrOpcode::CALL, type) {}

  explicit CallInstr(TypeId type, MethodId id, std::vector<Instr *> &&args)
      : Instr(InstrOpcode::CALL, type, std::move(args), {}), methodId_(id) {
    for (auto arg : args) {
      arg->AddUser(this);
    }
  }

  bool IsControllFlow() override final { return true; }

  void Dump(IrDumper &dumper) override;

private:
  MethodId methodId_;
};

} // namespace ir::instr

#endif