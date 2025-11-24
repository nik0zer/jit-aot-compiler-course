#ifndef PASS_H
#define PASS_H
#include "graph.h"

namespace passes {

class Pass {
public:
  virtual void Run(ir::MethodGraph *graph) = 0;
};

} // namespace passes

#endif