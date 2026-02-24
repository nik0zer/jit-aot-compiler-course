#include "binaryOperationInstr.h"
#include "instructions/instr.h"
#include "irDumper.h"
#include "macro.h"
#include <cstddef>
#include <string>

namespace ir::instr {
constexpr std::string_view BinaryOperationToString(BinaryOperationType op) {
  switch (op) {
  case BinaryOperationType::ADD:
    return "add";
  case BinaryOperationType::SUB:
    return "sub";
  case BinaryOperationType::MUL:
    return "mul";
  case BinaryOperationType::DIV:
    return "div";
  case BinaryOperationType::AND:
    return "and";
  case BinaryOperationType::OR:
    return "or";
  case BinaryOperationType::ASHR:
    return "ashr";
  }
  UNREACHABLE();
  return "";
}

void BinaryOperationInstr::Dump(IrDumper &dumper, bool dumpLiveness) {
  Instr::Dump(dumper, dumpLiveness);
  dumper.Add(" ");
  dumper.Add(BinaryOperationToString(op_).data());
  dumper.Add(" ");
  DumpInputs(dumper);
}

} // namespace ir::instr