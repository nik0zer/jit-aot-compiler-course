#include "irDumper.h"
#include <cstddef>
#include <string>

namespace ir {

void IrDumper::Add(const std::string &str) { ss_ << str; }
void IrDumper::Add(size_t num) { ss_ << std::to_string(num); }
void IrDumper::Add(long num) { ss_ << std::to_string(num); }
void IrDumper::Add(double num) { ss_ << std::to_string(num); }

void IrDumper::Endl() {
  ss_ << std::endl;
  Indent();
}

void IrDumper::IncreaseIndent() { ++indent_; }

void IrDumper::DecreaseIndent() {
  --indent_;
  if (indent_ < 0) {
    indent_ = 0;
  }
}

void IrDumper::SetIndent(size_t indent) {
  if (indent < 0) {
    indent = 0;
    return;
  }
  indent_ = indent;
}

void IrDumper::Indent() {
  for (size_t i = 0; i < indent_; ++i) {
    ss_ << "    ";
  }
}

std::string IrDumper::GetDump() { return ss_.str(); }

void IrDumper::Clear() {
  ss_.str("");
  SetIndent(0);
}

void IrDumper::Dump(std::ostream &os) { os << ss_.str(); }

} // namespace ir