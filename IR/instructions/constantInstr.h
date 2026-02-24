#ifndef CONSTANT_INSTR_H
#define CONSTANT_INSTR_H

#include "instr.h"
#include "type.h"
#include <cstddef>
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
    AdjustValueTypeByTypeId(type, value);
  }

  template <typename T> inline void SetValue(T value) {
    if (!CanGetValue<T>()) {
      type_ = GetTypeId<T>();
    }
    value_ = value;
  }

  template <typename T> inline void SetValue(TypeId type, T value) {
    type_ = type;
    AdjustValueTypeByTypeId(type, value);
  }

  template <typename T> inline T GetValue() const {
    T result = {}; // Локальная переменная для результата

    VisitTypeId(type_, [this, &result](auto ptr_type_tag) {
      using ConcreteType = std::remove_pointer_t<decltype(ptr_type_tag)>;
      if constexpr (!std::is_void_v<ConcreteType> &&
                    !std::is_same_v<ConcreteType, std::nullptr_t> &&
                    !std::is_same_v<ConcreteType, std::monostate>) {
        auto val = std::get<ConcreteType>(value_);
        result = static_cast<T>(val);
      }
    });
    return result;
  }

  template <typename T> inline bool CanGetValue() const {
    return std::holds_alternative<T>(value_);
  }

  void Dump(IrDumper &dumper, bool dumpLiveness = false) override;

private:
  template <typename T> void AdjustValueTypeByTypeId(TypeId type, T value) {
    VisitTypeId(type, [this, &value](auto ptr_type_tag) {
      using ConcreteType = std::remove_pointer_t<decltype(ptr_type_tag)>;
      if constexpr (!std::is_same_v<ConcreteType, void> &&
                    !std::is_same_v<ConcreteType, std::nullptr_t>) {
        value_ = static_cast<ConcreteType>(value);
      }
    });
  };
  std::variant<std::monostate, uint8_t, uint16_t, uint32_t, uint64_t, int8_t,
               int16_t, int32_t, int64_t, float, double, void *, std::nullptr_t>
      value_;
};

} // namespace ir::instr

#endif