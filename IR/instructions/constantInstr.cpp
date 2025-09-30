#include "constantInstr.h"
#include "instructions/instr.h"
#include "macro.h"

namespace ir::instr {

void ConstantInstr::Dump(IrDumper &dumper) {
  Instr::Dump(dumper);
  dumper.Add(" const ");

  std::visit(
      [&dumper](auto &&value) {
        using T = std::decay_t<decltype(value)>;

        if constexpr (std::is_same_v<T, std::monostate>) {
          UNREACHABLE();
        } else if constexpr (std::is_floating_point_v<T>) {
          dumper.Add(static_cast<double>(value));
        } else if constexpr (std::is_signed_v<T>) {
          dumper.Add(static_cast<long>(value));
        } else if constexpr (std::is_unsigned_v<T>) {
          dumper.Add(static_cast<size_t>(value));
        }
      },
      value_);
}

} // namespace ir::instr