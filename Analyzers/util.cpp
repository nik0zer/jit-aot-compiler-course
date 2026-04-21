#include "util.h"
#include "basicBlock.h"
#include "instructions/instr.h"

namespace analyzer {

bool InstructionsDominates(ir::instr::Instr *dominator,
                           ir::instr::Instr *dominatee) {
  auto dominatorBB = dominator->GetBB();
  auto dominateeBB = dominatee->GetBB();

  if (dominatorBB == dominateeBB) {
    auto current = dominator->GetNextInstr();
    while (current != nullptr) {
      if (current == dominatee) {
        return true;
      }
      current = current->GetNextInstr();
    }
    return false;
  }

  auto &dominateeDominators = dominateeBB->GetDominators();
  for (auto &bb_pair : dominateeDominators) {
    if (bb_pair.first == dominatorBB) {
      return true;
    }
  }

  return false;
}

} // namespace analyzer