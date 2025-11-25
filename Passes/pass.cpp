#include "pass.h"
#include "basicBlock.h"
namespace passes {

void Pass::ReplaceInstr(ir::BasicBlock *block, ir::instr::Instr *instr,
                        ir::instr::Instr *newInstr) {
  std::vector<ir::instr::Instr *> users(instr->GetUsers().begin(),
                                        instr->GetUsers().end());
  for (auto user : users) {
    user->ReplaceInput(instr, newInstr);
  }
  block->RemoveInstr(instr);
}

} // namespace passes