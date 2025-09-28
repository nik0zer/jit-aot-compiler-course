#include "binaryOperationInstr.h"
#include "macro.h"
#include <string>

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
}
} // namespace ir::instr