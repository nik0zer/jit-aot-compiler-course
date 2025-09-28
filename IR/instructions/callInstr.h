#ifndef CALL_INSTR_H
#define CALL_INSTR_H

#include "graph.h"
#include "instr.h"
#include <vector>

namespace ir::instr {

class CallInstr : public Instr {
public:
  explicit CallInstr(TypeId type) : Instr(InstrOpcode::CALL, type) {}

  explicit CallInstr(MethodId id, TypeId type, std::vector<Instr *> &&args)
      : Instr(InstrOpcode::CALL, type, std::move(args), {}), id_(id) {
    for (auto arg : args) {
      arg->AddUser(this);
    }
  }

  bool IsControllFlow() override final { return true; }

private:
  MethodId id_;
};

} // namespace ir::instr

#endif