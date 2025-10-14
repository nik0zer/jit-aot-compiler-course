#ifndef TYPE_H
#define TYPE_H

#include "macro.h"
#include <cstdint>
#include <sys/types.h>
#include <type_traits>
namespace ir::instr {
enum class TypeId { U8, U16, U32, U64, I8, I16, I32, I64, F32, F64 };

template <typename T> constexpr inline TypeId GetTypeId() {
  if constexpr (std::is_same_v<T, uint8_t>) {
    return TypeId::U8;
  } else if constexpr (std::is_same_v<T, uint16_t>) {
    return TypeId::U16;
  } else if constexpr (std::is_same_v<T, uint32_t>) {
    return TypeId::U32;
  } else if constexpr (std::is_same_v<T, uint64_t>) {
    return TypeId::U64;
  } else if constexpr (std::is_same_v<T, int8_t>) {
    return TypeId::I8;
  } else if constexpr (std::is_same_v<T, int16_t>) {
    return TypeId::I16;
  } else if constexpr (std::is_same_v<T, int32_t>) {
    return TypeId::I32;
  } else if constexpr (std::is_same_v<T, int64_t>) {
    return TypeId::I64;
  } else if constexpr (std::is_same_v<T, float>) {
    return TypeId::F32;
  } else if constexpr (std::is_same_v<T, double>) {
    return TypeId::F64;
  }
  UNREACHABLE();
}

template <TypeId Id> struct TypeIdToType;

template <TypeId Id> using TypeIdToType_t = typename TypeIdToType<Id>::type;

template <> struct TypeIdToType<TypeId::U8> { using type = uint8_t; };
template <> struct TypeIdToType<TypeId::U16> { using type = uint16_t; };
template <> struct TypeIdToType<TypeId::U32> { using type = uint32_t; };
template <> struct TypeIdToType<TypeId::U64> { using type = uint64_t; };
template <> struct TypeIdToType<TypeId::I8> { using type = int8_t; };
template <> struct TypeIdToType<TypeId::I16> { using type = int16_t; };
template <> struct TypeIdToType<TypeId::I32> { using type = int32_t; };
template <> struct TypeIdToType<TypeId::I64> { using type = int64_t; };
template <> struct TypeIdToType<TypeId::F32> { using type = float; };
template <> struct TypeIdToType<TypeId::F64> { using type = double; };

inline const std::string_view TypeIdToString(TypeId type) {
  switch (type) {
  case TypeId::U8:
    return "u8";
  case TypeId::U16:
    return "u16";
  case TypeId::U32:
    return "u32";
  case TypeId::U64:
    return "u64";
  case TypeId::I8:
    return "i8";
  case TypeId::I16:
    return "i16";
  case TypeId::I32:
    return "i32";
  case TypeId::I64:
    return "i64";
  case TypeId::F32:
    return "f32";
  case TypeId::F64:
    return "f64";
  default:
    UNREACHABLE();
  }
  return "";
}

template <typename Visitor> auto VisitTypeId(TypeId type, Visitor &&visitor) {
  switch (type) {
  case TypeId::U8:
    return visitor(static_cast<uint8_t *>(nullptr));
  case TypeId::U16:
    return visitor(static_cast<uint16_t *>(nullptr));
  case TypeId::U32:
    return visitor(static_cast<uint32_t *>(nullptr));
  case TypeId::U64:
    return visitor(static_cast<uint64_t *>(nullptr));
  case TypeId::I8:
    return visitor(static_cast<int8_t *>(nullptr));
  case TypeId::I16:
    return visitor(static_cast<int16_t *>(nullptr));
  case TypeId::I32:
    return visitor(static_cast<int32_t *>(nullptr));
  case TypeId::I64:
    return visitor(static_cast<int64_t *>(nullptr));
  case TypeId::F32:
    return visitor(static_cast<float *>(nullptr));
  case TypeId::F64:
    return visitor(static_cast<double *>(nullptr));
  }
  UNREACHABLE(); // В случае если придет невалидный TypeId
}

} // namespace ir::instr

#endif