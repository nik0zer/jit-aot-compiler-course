#ifndef CALL_INSTR_H
#define CALL_INSTR_H

#include "graph.h"
#include "instr.h"
#include <vector>

namespace ir::instr {

class CallInstr : public Instr {
public:
  explicit CallInstr() : Instr(InstrOpcode::CALL) {}

  explicit CallInstr(MethodId id, std::vector<Instr *> &&args)
      : Instr(InstrOpcode::CALL, std::move(args), {}), id_(id) {
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