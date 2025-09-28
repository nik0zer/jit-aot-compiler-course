#ifndef IR_DUMPER
#define IR_DUMPER

#include <sstream>
namespace ir {

class IrDumper {
public:
  IrDumper() {}

private:
  std::stringstream ss_{};
};

} // namespace ir

#endif