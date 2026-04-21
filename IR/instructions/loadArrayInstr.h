#ifndef LD_ARR_INSTR
#define LD_ARR_INSTR

#include "instr.h"
#include "instructions/type.h"

namespace ir::instr {
class LoadArrayInstr : public Instr {
public:
  LoadArrayInstr(TypeId type, Instr *array, Instr *index)
      : Instr(InstrOpcode::LD_ARR, type) {
    AddInput(array);
    AddInput(index);

    if (array != nullptr) {
      array->AddUser(this);
    }
    if (index != nullptr) {
      index->AddUser(this);
    }
  }

  void Dump(IrDumper &dumper, bool dumpLiveness = false) override;
};

} // namespace ir::instr

#endif