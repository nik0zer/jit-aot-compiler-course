#ifndef CONSTANT_INSTR_H
#define CONSTANT_INSTR_H

#include "instr.h"
#include <cstdint>
#include <variant>

namespace ir::instr {

class ConstantInstr : public Instr {
public:
  explicit ConstantInstr(TypeId type) : Instr(InstrOpcode::CONSTANT, type) {}

  


private:
  std::variant<std::monostate, uint8_t, uint16_t, uint32_t, uint64_t, int8_t, int16_t,
  int32_t, int64_t, float, double> value_;
};

} // namespace ir::instr

#endif