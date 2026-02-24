#ifndef IRBUILDER_H
#define IRBUILDER_H

#include "graph.h"
#include <atomic>
#include <optional>
#include <ostream>

namespace IrBuilder {

class IrBuilder {
public:
  IrBuilder() = default;
  ~IrBuilder() = default;

  virtual ir::MethodGraph *Build(std::ostream &diagnosticOutput) = 0;
};

} // namespace IrBuilder

#endif