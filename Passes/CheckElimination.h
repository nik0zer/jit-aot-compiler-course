#ifndef CHECK_ELIMINATION_H
#define CHECK_ELIMINATION_H

#include "pass.h"

namespace ir {
class MethodGraph;
}

namespace passes {
class CheckElimination : public Pass {
public:
  explicit CheckElimination(bool dominatorsFound = false)
      : dominatorsFound_(dominatorsFound) {}

  void Run(ir::MethodGraph *graph) override;

  bool GetDominatorsFound() const { return dominatorsFound_; }
  void SetDominatorsFound(bool dominatorsFound) {
    dominatorsFound_ = dominatorsFound;
  }

private:
  bool dominatorsFound_;
  ir::MethodGraph *graph_;

  void EliminateRedundantChecks();
  void ProcessNullInstruction(ir::instr::Instr *null_instr);
  void ProcessNullCheck(ir::instr::Instr *check_null_instr);
  void ProcessBoundsCheck(ir::instr::Instr *check_bounds_instr);
};
} // namespace passes

#endif // CHECK_ELIMINATION_H