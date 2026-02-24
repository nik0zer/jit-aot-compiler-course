#ifndef PARAM_INSTR_H
#define PARAM_INSTR_H

#include "graph.h"
#include "instr.h"

namespace ir::instr {

class ParamInstr : public Instr {
public:
  explicit ParamInstr(TypeId type, ParamId id)
      : Instr(InstrOpcode::PARAM, type), id_(id) {}

  void Dump(IrDumper &dumper, bool dumpLiveness = false) override;

private:
  ParamId id_;
};

} // namespace ir::instr

#endif