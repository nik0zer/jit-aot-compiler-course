#include "peepholes.h"
#include "basicBlock.h"
#include "dominatorAnalyzer.h"
#include "graph.h"
#include "instructions/binaryOperationInstr.h"
#include "instructions/castInstr.h"
#include "instructions/constantInstr.h"
#include "instructions/instr.h"
#include "instructions/type.h"
#include "irDumper.h"
#include "pass.h"
#include <set>
#include <vector>

namespace passes {

void PeepHoles::Run(ir::MethodGraph *graph) {
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

static bool CheckAndApplyPeephole(ir::BasicBlock *block,
                                  ir::instr::Instr *instr,
                                  ir::instr::TypeId typeId,
                                  ir::instr::ConstantInstr *constInput,
                                  ir::instr::Instr *realInput) {
  return VisitTypeId(typeId, [&](auto ptr_type_tag) {
    using T = std::remove_pointer_t<decltype(ptr_type_tag)>;
    if constexpr (!std::is_void_v<T> && !std::is_same_v<T, std::nullptr_t>) {
      auto constVal = constInput->GetValue<T>();
      T result;
      switch (instr->AsBinaryOperationInstr()->GetOperation()) {
      case ir::instr::BinaryOperationType::ADD:
      case ir::instr::BinaryOperationType::OR:
        // x | 0 == x
        // 0 | x == x
        // x + 0 == x
        // 0 + x == x
        if (constVal == 0) {
          Pass::ReplaceInstr(block, instr, realInput);
          return true;
        }
        break;
      case ir::instr::BinaryOperationType::ASHR:
        if (constInput != instr->GetInputs()[1]) {
          break;
        }
        // x >> (y >= max_digits) = 0
        if (constVal >= ir::instr::TypeIdToSize(typeId)) {
          auto zeroConst = block->AllocateInstrAfter<ir::instr::ConstantInstr>(
              instr, typeId, 0);
          Pass::ReplaceInstr(block, instr, zeroConst);
          return true;
        }
        // x >> 0 = x
        if (constVal == 0) {
          Pass::ReplaceInstr(block, instr, realInput);
          return true;
        }
        break;
      default:
        return false;
      }
    }
    return false;
  });
}

bool PeepHoles::processBlock(ir::BasicBlock *block) {
  auto changed = false;
  auto instr = block->GetFirstInstr();
  while (instr != nullptr) {
    auto nextInstr = instr->GetNextInstr();
    switch (instr->GetOpcode()) {
    case ir::instr::InstrOpcode::BINARY_OPERATION: {
      if (instr->AsBinaryOperationInstr()->GetOperation() ==
          ir::instr::BinaryOperationType::OR) {
        // x | x == x
        if (instr->GetInputs()[0] == instr->GetInputs()[1]) {
          Pass::ReplaceInstr(block, instr, instr->GetInputs()[0]);
          changed |= true;
          break;
        }
      }
      if (instr->GetInputs()[0]->IsConstantInstr()) {
        auto constInput = instr->GetInputs()[0]->AsConstantInstr();
        auto realinput = instr->GetInputs()[1];
        auto typeId = instr->GetType();
        changed |=
            CheckAndApplyPeephole(block, instr, typeId, constInput, realinput);
        break;
      }
      if (instr->GetInputs()[1]->IsConstantInstr()) {
        auto constInput = instr->GetInputs()[1]->AsConstantInstr();
        auto realinput = instr->GetInputs()[0];
        auto typeId = instr->GetType();
        changed |=
            CheckAndApplyPeephole(block, instr, typeId, constInput, realinput);
        break;
      }
      break;
    }
    default:
      break;
    }
    instr = nextInstr;
  }
  return changed;
}

} // namespace passes