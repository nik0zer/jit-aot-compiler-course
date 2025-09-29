#ifndef CONSTANT_INSTR_H
#define CONSTANT_INSTR_H

#include "instr.h"
#include "type.h"
#include <cstdint>
#include <variant>

namespace ir::instr {

class ConstantInstr : public Instr {
public:
  template <typename T>
  explicit ConstantInstr(T value)
      : Instr(InstrOpcode::CONSTANT, GetTypeId<T>()) {
    value_ = value;
  }

  template <typename T>
  explicit ConstantInstr(TypeId type, T value)
      : Instr(InstrOpcode::CONSTANT, type) {
    value_ = value;
  }

  template <typename T> inline void SetValue(T value) {
    if (!CanGetValue<T>()) {
      type_ = GetTypeId<T>();
    }
    value_ = value;
  }

  template <typename T> inline T GetValue() const {
    ASSERT(CanGetValue<T>());
    return std::get<T>(value_);
  }

  template <typename T> inline bool CanGetValue() const {
    return std::holds_alternative<T>(value_);
  }

  void Dump(IrDumper &dumper) override;

private:
  std::variant<std::monostate, uint8_t, uint16_t, uint32_t, uint64_t, int8_t,
               int16_t, int32_t, int64_t, float, double>
      value_;
};

} // namespace ir::instr

#endif