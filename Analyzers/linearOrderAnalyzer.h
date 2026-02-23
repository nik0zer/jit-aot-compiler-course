#ifndef LINEAR_ORDER_ANALYZER
#define LINEAR_ORDER_ANALYZER

#include "basicBlock.h"
#include "graph.h"

namespace analyzer {
class LinearOrderAnalyzer {
public:
  explicit LinearOrderAnalyzer(ir::MethodGraph &graph) : graph_(graph) {}

  void CreateLinearOrder(ir::BasicBlock *startBlock = nullptr);
  void Dump(ir::IrDumper &dumper);

private:
  ir::MethodGraph &graph_;
};

} // namespace analyzer


#endif