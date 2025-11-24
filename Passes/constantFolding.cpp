#include "constantFolding.h"
#include "basicBlock.h"
#include "dominatorAnalyzer.h"
#include "graph.h"
#include "instructions/binaryOperationInstr.h"
#include "instructions/castInstr.h"
#include "instructions/constantInstr.h"
#include "instructions/instr.h"
#include "irDumper.h"
#include <set>
#include <vector>

namespace passes {

void ConstantFolding::Run(ir::MethodGraph *graph) {
  graph_ = graph;
  auto domAnalyzer = analyzer::DominatorAnalyzer(*graph);
  auto rpo = domAnalyzer.RPO();

  bool changed = false;
  do {
    changed = false;
    for (auto block : rpo) {
      changed |= processBlock(block.first);
    }
  } while (changed);
}

static void ChangeUsersInputs(ir::BasicBlock *block, ir::instr::Instr *instr,
                              ir::instr::Instr *newInstr) {
  std::vector<ir::instr::Instr *> users(instr->GetUsers().begin(),
                                        instr->GetUsers().end());
  for (auto user : users) {
    user->ReplaceInput(instr, newInstr);
  }
  block->RemoveInstr(instr);
}

bool ConstantFolding::processBlock(ir::BasicBlock *block) {
  auto changed = false;
  auto instr = block->GetFirstInstr();
  while (instr != nullptr) {
    auto nextInstr = instr->GetNextInstr();
    switch (instr->GetOpcode()) {
    case ir::instr::InstrOpcode::BINARY_OPERATION: {
      switch (instr->AsBinaryOperationInstr()->GetOperation()) {
      case ir::instr::BinaryOperationType::ADD:
      case ir::instr::BinaryOperationType::OR:
      case ir::instr::BinaryOperationType::ASHR:
      default:
        break;
      }
    }
    case ir::instr::InstrOpcode::CAST: {
      if (instr->GetInputs()[0]->IsConstantInstr()) {
        changed |= true;
        auto processedConst = instr->GetInputs()[0]->AsConstantInstr();
        auto typeId = instr->GetType();
        VisitTypeId(typeId, [&](auto ptr_type_tag) {
          using T = std::remove_pointer_t<decltype(ptr_type_tag)>;
          if constexpr (!std::is_void_v<T> &&
                        !std::is_same_v<T, std::nullptr_t>) {
            auto val = processedConst->GetValue<T>();
            auto newInstr = block->AllocateInstrAfter<ir::instr::ConstantInstr>(
                instr, typeId, val);
            ChangeUsersInputs(block, instr, newInstr);
          }
        });
      }
    }
    default:
      break;
    }
    instr = nextInstr;
  }
  return changed;
}

} // namespace passes