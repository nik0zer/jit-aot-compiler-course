#ifndef PASS_H
#define PASS_H
#include "graph.h"

namespace passes {

class Pass {
public:
  virtual void Run(ir::MethodGraph *graph) = 0;

  static void ReplaceInstr(ir::BasicBlock *block, ir::instr::Instr *instr,
                           ir::instr::Instr *newInstr);
};

} // namespace passes

#endif