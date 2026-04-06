#ifndef INLINE_PASS_H
#define INLINE_PASS_H

#include "basicBlock.h"
#include "pass.h"
#include <cstddef>
#include <functional>
#include <string>

namespace ir {
class MethodGraph;
}

namespace passes {

constexpr std::size_t MAX_NUMBER_OF_INSTRS_TO_INLINE = 50;

class InlinePass : public Pass {
public:
  explicit InlinePass(
      const std::function<ir::MethodGraph *(const std::string &)>
          &methodGraphGenerator)
      : methodGraphGenerator_(methodGraphGenerator) {}

  void Run(ir::MethodGraph *graph) override;

private:
  void TryInlineStaticCall(ir::MethodGraph *callerGraph, ir::BasicBlock *bb,
                           ir::instr::CallStaticInstr *callInstr,
                           const std::string &methodName);
  ir::BasicBlock *SplitBlock(ir::MethodGraph *callerGraph, ir::BasicBlock *bb,
                             ir::instr::Instr *callInstr);

  const std::function<ir::MethodGraph *(const std::string &)>
      methodGraphGenerator_;
};

} // namespace passes

#endif // INLINE_PASS_H
