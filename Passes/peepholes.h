#ifndef CONSTANT_FOLDING_H
#define CONSTANT_FOLDING_H

#include "instructions/instr.h"
#include "pass.h"

namespace passes {

class PeepHoles : public Pass {
public:
  void Run(ir::MethodGraph *graph) override;

private:
  bool processBlock(ir::BasicBlock *block);

  ir::MethodGraph *graph_;
};

} // namespace passes

#endif