#ifndef IR_DUMPER
#define IR_DUMPER

#include <cstddef>
#include <ios>
#include <ostream>
#include <sstream>
namespace ir {

class IrDumper {
public:
  IrDumper() {}
  IrDumper(size_t indent) : indent_(indent) {}

  void Add(const std::string &str);
  void Add(size_t num);
  void Add(long num);
  void Endl();
  void IncreaseIndent();
  void DecreaseIndent();
  std::string GetDump();
  void Clear();
  void Dump(std::ostream &os);

private:
  void Indent();

  size_t indent_ = 0;
  std::stringstream ss_{};
};

} // namespace ir

#endif