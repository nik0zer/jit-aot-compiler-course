#include "ifInstr.h"

namespace ir::instr {

constexpr std::string_view IfTypeToString(IfType type) {
  switch (type) {
  case IfType::EQ:
    return "eq";
  case IfType::NE:
    return "ne";
  case IfType::LT:
    return "lt";
  case IfType::LE:
    return "le";
  case IfType::GT:
    return "gt";
  case IfType::GE:
    return "ge";
  }
  UNREACHABLE();
  return "";
}

void IfInstr::Dump(IrDumper &dumper) {
  Instr::Dump(dumper);
  dumper.Add(" if.");
  dumper.Add(IfTypeToString(type_).data());
  dumper.Add(" ");
  DumpInputs(dumper);
}

} // namespace ir::instr