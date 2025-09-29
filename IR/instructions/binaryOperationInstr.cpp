#include "binaryOperationInstr.h"
#include "instructions/instr.h"
#include "macro.h"
#include <cstddef>
#include <string>
#include "irDumper.h"

namespace ir::instr {
constexpr std::string_view BinaryOperationToString(BinaryOperation op) {
  switch (op) {
  case BinaryOperation::ADD:
    return "add";
  case BinaryOperation::SUB:
    return "sub";
  case BinaryOperation::MUL:
    return "mul";
  case BinaryOperation::DIV:
    return "div";
  case BinaryOperation::AND:
    return "and";
  case BinaryOperation::OR:
    return "or";
  }
  UNREACHABLE();
  return "";
}

void BinaryOperationInstr::Dump(IrDumper &dumper) {
  Instr::Dump(dumper);
  dumper.Add(" ");
  dumper.Add(BinaryOperationToString(op_).data());
  dumper.Add(" ");
  DumpInputs(dumper);
}

} // namespace ir::instr