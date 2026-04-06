#ifndef CALL_INSTR_H
#define CALL_INSTR_H

#include "graph.h"
#include "instr.h"
#include <string>
#include <string_view>
#include <vector>

namespace ir::instr {

class CallStaticInstr : public Instr {
public:
  explicit CallStaticInstr(TypeId type)
      : Instr(InstrOpcode::CALL_STATIC, type) {}

  explicit CallStaticInstr(TypeId type, std::string_view methodName,
                           const std::vector<Instr *> &args)
      : Instr(InstrOpcode::CALL_STATIC, type, args, {}),
        methodName_(methodName) {
    for (auto arg : inputs_) {
      arg->AddUser(this);
    }
  }
  explicit CallStaticInstr(TypeId type, std::string_view methodName,
                           std::vector<Instr *> &&args)
      : Instr(InstrOpcode::CALL_STATIC, type, args, {}),
        methodName_(methodName) {
    for (auto arg : inputs_) {
      arg->AddUser(this);
    }
  }

  bool IsControllFlow() override final { return true; }

  void Dump(IrDumper &dumper, bool dumpLiveness = false) override;

  const std::string &MethodName() const { return methodName_; }

private:
  std::string methodName_;
};

} // namespace ir::instr

#endif