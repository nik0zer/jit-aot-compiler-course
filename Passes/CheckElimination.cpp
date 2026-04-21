#include "CheckElimination.h"
#include "basicBlock.h"
#include "dominatorAnalyzer.h"
#include "instructions/constantInstr.h"
#include "instructions/instr.h"
#include "instructions/phiInstr.h"
#include "instructions/type.h"
#include <iostream>
#include <ostream>

namespace {
static bool AreChecksEqual(ir::instr::Instr *check1, ir::instr::Instr *check2) {
  if (check1->GetOpcode() != check2->GetOpcode()) {
    return false;
  }

  auto inputs1 = check1->GetInputs();
  auto inputs2 = check2->GetInputs();

  if (inputs1.size() != inputs2.size()) {
    return false;
  }

  for (size_t i = 0; i < inputs1.size(); ++i) {
    auto input1 = inputs1[i];
    auto input2 = inputs2[i];

    if (input1 == input2) {
      continue;
    }

    if (input1->IsConstantInstr() && input2->IsConstantInstr()) {
      auto const1 = input1->AsConstantInstr();
      auto const2 = input2->AsConstantInstr();

      if (const1->GetType() != const2->GetType()) {
        return false;
      }

      bool areEqual =
          ir::instr::VisitTypeId(const1->GetType(), [&](auto ptr_type_tag) {
            using T = std::remove_pointer_t<decltype(ptr_type_tag)>;
            if constexpr (!std::is_void_v<T> &&
                          !std::is_same_v<T, std::nullptr_t> &&
                          !std::is_same_v<T, void *>) {
              return const1->GetValue<T>() == const2->GetValue<T>();
            }
            return false;
          });

      if (!areEqual) {
        return false;
      }
      continue;
    }
    return false;
  }

  return true;
}

static bool
CheckDominanceAndRemove(const std::vector<analyzer::BlockWithIndex> &dominators,
                        ir::BasicBlock *succ, ir::instr::Instr *user,
                        size_t succ_to_remove) {
  if (succ == nullptr) {
    return false;
  }
  auto remove = [&]() {
    user->GetBB()->RemoveSuccessor(succ_to_remove);
    for (auto input : user->GetInputs()) {
      input->RemoveUser(user);
    }
    user->GetBB()->RemoveInstr(user);
  };
  if (succ == user->GetBB()) {
    remove();
    return true;
  }
  for (auto &dom_pair : dominators) {
    if (dom_pair.first == succ) {
      remove();
      return true;
    }
  }
  return false;
}
} // anonymous namespace

namespace passes {

void CheckElimination::ProcessNullInstruction(ir::instr::Instr *null_instr) {
  std::vector<ir::instr::Instr *> users(null_instr->GetUsers().begin(),
                                        null_instr->GetUsers().end());
  for (auto user : users) {
    if (user->GetOpcode() == ir::instr::InstrOpcode::CHECK_NULL) {
      auto checkNullBB = user->GetBB();
      checkNullBB->RemoveSuccessor(1);
      for (auto input : user->GetInputs()) {
        input->RemoveUser(user);
      }
      checkNullBB->RemoveInstr(user);
    }
  }
}

void CheckElimination::ProcessNullCheck(ir::instr::Instr *check_null_instr) {
  auto input = check_null_instr->GetInputs()[0];
  auto users = input->GetUsers();
  auto check_null_bb = check_null_instr->GetBB();

  for (auto user : users) {
    if (user->GetOpcode() == ir::instr::InstrOpcode::CHECK_NULL &&
        user != check_null_instr) {
      auto other_check_null_bb = user->GetBB();
      auto other_dominators = other_check_null_bb->GetDominators();

      if (CheckDominanceAndRemove(other_dominators,
                                  check_null_bb->GetSuccs()[0], user, 1)) {
        continue;
      }
      CheckDominanceAndRemove(other_dominators, check_null_bb->GetSuccs()[1],
                              user, 0);
    }
  }
}

void CheckElimination::ProcessBoundsCheck(
    ir::instr::Instr *check_bounds_instr) {
  auto input = check_bounds_instr->GetInputs()[0];
  auto users = input->GetUsers();
  auto check_bounds_bb = check_bounds_instr->GetBB();

  for (auto user : users) {
    if (user->GetOpcode() == ir::instr::InstrOpcode::CHECK_BOUNDS &&
        user != check_bounds_instr &&
        AreChecksEqual(check_bounds_instr, user)) {
      auto other_check_bounds_bb = user->GetBB();
      auto other_dominators = other_check_bounds_bb->GetDominators();

      if (CheckDominanceAndRemove(other_dominators,
                                  check_bounds_bb->GetSuccs()[0], user, 1)) {
        continue;
      }
      CheckDominanceAndRemove(other_dominators, check_bounds_bb->GetSuccs()[1],
                              user, 0);
    }
  }
}

void CheckElimination::EliminateRedundantChecks() {
  auto domAnalyzer = analyzer::DominatorAnalyzer(*graph_);
  if (dominatorsFound_) {
    for (auto block : graph_->GetBlocks()) {
      if (block->GetPreds().empty()) {
        domAnalyzer.FindDominators(block);
      }
    }
  }
  auto rpo = domAnalyzer.RPO();

  for (auto &block_pair : rpo) {
    auto block = block_pair.first;
    for (auto instr = block->GetFirstInstr(); instr != nullptr;
         instr = instr->GetNextInstr()) {
      if (instr->GetOpcode() == ir::instr::InstrOpcode::NULL_INSTR) {
        ProcessNullInstruction(instr);
        continue;
      }
      if (instr->GetOpcode() == ir::instr::InstrOpcode::CHECK_NULL) {
        ProcessNullCheck(instr);
        continue;
      }
      if (instr->GetOpcode() == ir::instr::InstrOpcode::CHECK_BOUNDS) {
        ProcessBoundsCheck(instr);
        continue;
      }
    }
  }
}

void CheckElimination::Run(ir::MethodGraph *graph) {
  graph_ = graph;
  EliminateRedundantChecks();
}
} // namespace passes