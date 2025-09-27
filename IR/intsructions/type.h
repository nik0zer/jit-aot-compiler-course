#include <cstdint>
#include <sys/types.h>
#include <type_traits>
namespace ir::instr {
enum class TypeId {
  U8,
  U16,
  U32,
  U64,
  I8,
  I16,
  I32,
  I64,
  F32,
  F64
};

template<typename T>
constexpr TypeId GetType()
{
    if constexpr (std::is_same_v<T, uint8_t>) {
        return TypeId::U8;
    }
    else if constexpr (std::is_same_v<T, uint16_t>) {
        return TypeId::U16;
    }
    else if constexpr (std::is_same_v<T, uint32_t>) {
        return TypeId::U32;
    }
    else if constexpr (std::is_same_v<T, uint64_t>) {
        return TypeId::U64;
    }
    else if constexpr (std::is_same_v<T, int8_t>) {
        return TypeId::I8;
    }
    else if constexpr (std::is_same_v<T, int16_t>) {
        return TypeId::I16;
    }
    else if constexpr (std::is_same_v<T, int32_t>) {
        return TypeId::I32;
    }
    else if constexpr (std::is_same_v<T, int64_t>) {
        return TypeId::I64;
    }
    else if constexpr (std::is_same_v<T, float>) {
        return TypeId::F32;
    }
    else if constexpr (std::is_same_v<T, double>) {
        return TypeId::F64;
    }
    // UNREACHABLE();
}

} // namespace ir::instr