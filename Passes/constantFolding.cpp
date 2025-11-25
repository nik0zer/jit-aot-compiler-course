#include "constantFolding.h"
#include "basicBlock.h"
#include "dominatorAnalyzer.h"
#include "graph.h"
#include "instructions/binaryOperationInstr.h"
#include "instructions/castInstr.h"
#include "instructions/constantInstr.h"
#include "instructions/instr.h"
#include "irDumper.h"
#include "pass.h"
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

bool ConstantFolding::processBlock(ir::BasicBlock *block) {
  auto changed = false;
  auto instr = block->GetFirstInstr();
  while (instr != nullptr) {
    auto nextInstr = instr->GetNextInstr();
    switch (instr->GetOpcode()) {
    case ir::instr::InstrOpcode::BINARY_OPERATION: {
      if (instr->GetInputs()[0]->IsConstantInstr() &&
          instr->GetInputs()[1]->IsConstantInstr()) {
        changed |= true;
        auto leftConst = instr->GetInputs()[0]->AsConstantInstr();
        auto rightConst = instr->GetInputs()[1]->AsConstantInstr();
        auto typeId = instr->GetType();
        VisitTypeId(typeId, [&](auto ptr_type_tag) {
          using T = std::remove_pointer_t<decltype(ptr_type_tag)>;
          if constexpr (!std::is_void_v<T> &&
                        !std::is_same_v<T, std::nullptr_t>) {
            auto leftVal = leftConst->GetValue<T>();
            auto rightVal = rightConst->GetValue<T>();
            T result;
            switch (instr->AsBinaryOperationInstr()->GetOperation()) {
            case ir::instr::BinaryOperationType::ADD:
              result = leftVal + rightVal;
              break;
            case ir::instr::BinaryOperationType::OR:
              if constexpr (!std::is_floating_point_v<T>) {
                // OR is not defined for floating point types
                result = leftVal | rightVal;
              }
              break;
            case ir::instr::BinaryOperationType::ASHR:
              if constexpr (!std::is_floating_point_v<T>) {
                // ASHR is not defined for floating point types
                result = leftVal >> rightVal;
              }
              break;
            default:
              return;
            }
            auto newInstr = block->AllocateInstrAfter<ir::instr::ConstantInstr>(
                instr, typeId, result);
            Pass::ReplaceInstr(block, instr, newInstr);
          }
        });
      }
      break;
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
            Pass::ReplaceInstr(block, instr, newInstr);
          }
        });
      }
      break;
    }
    case ir::instr::InstrOpcode::PHI: {
      bool allConst = true;
      for (auto input : instr->GetInputs()) {
        if (!input->IsConstantInstr()) {
          allConst = false;
          break;
        }
      }
      if (allConst) {
        changed |= true;
        auto typeId = instr->GetType();
        VisitTypeId(typeId, [&](auto ptr_type_tag) {
          using T = std::remove_pointer_t<decltype(ptr_type_tag)>;
          if constexpr (!std::is_void_v<T> &&
                        !std::is_same_v<T, std::nullptr_t>) {
            auto firstConst = instr->GetInputs()[0]->AsConstantInstr();
            auto val = firstConst->GetValue<T>();
            auto newInstr = block->AllocateInstrAfter<ir::instr::ConstantInstr>(
                instr, typeId, val);
            Pass::ReplaceInstr(block, instr, newInstr);
          }
        });
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