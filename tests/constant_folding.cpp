#include "basicBlock.h"
#include "constantFolding.h"
#include "graph.h"
#include "instructions/binaryOperationInstr.h"
#include "instructions/castInstr.h"
#include "instructions/constantInstr.h"
#include "instructions/ifInstr.h"
#include "instructions/instr.h"
#include "instructions/paramInstr.h"
#include "instructions/phiInstr.h"
#include "instructions/returnInstr.h"
#include "instructions/type.h"
#include "irDumper.h"
#include "test_utils/util.h"
#include <cstddef>
#include <iostream>

int main() {
  auto graph = ir::MethodGraph("TestOptimizerPasses_1", 0);
  auto bbStart = graph.AllocateBlock();

  auto bbCast = graph.AllocateBlock();
  bbCast->SetPreds({bbStart});
  bbStart->SetSuccs({bbCast});

  auto constU32_1 = bbCast->AllocateInstr<ir::instr::ConstantInstr>(
      ir::instr::TypeId::U32, 1);
  auto constU32_2 = bbCast->AllocateInstr<ir::instr::ConstantInstr>(
      ir::instr::TypeId::U32, 2);
  auto constU32_7 = bbCast->AllocateInstr<ir::instr::ConstantInstr>(
      ir::instr::TypeId::U32, 7);
  auto castInstr_1 = bbCast->AllocateInstr<ir::instr::CastInstr>(
      ir::instr::TypeId::U64, constU32_1);
  auto castInstr_2 = bbCast->AllocateInstr<ir::instr::CastInstr>(
      ir::instr::TypeId::U64, constU32_2);
  auto castInstr_7 = bbCast->AllocateInstr<ir::instr::CastInstr>(
      ir::instr::TypeId::U64, constU32_7);
  auto add_1 = bbCast->AllocateInstr<ir::instr::BinaryOperationInstr>(
      ir::instr::TypeId::U64, ir::instr::BinaryOperationType::ADD, castInstr_1,
      castInstr_7);
  auto add_2 = bbCast->AllocateInstr<ir::instr::BinaryOperationInstr>(
      ir::instr::TypeId::U64, ir::instr::BinaryOperationType::ADD, castInstr_7,
      add_1);
  auto or_1 = bbCast->AllocateInstr<ir::instr::BinaryOperationInstr>(
      ir::instr::TypeId::U64, ir::instr::BinaryOperationType::OR, castInstr_1,
      castInstr_7);
  auto add_3 = bbCast->AllocateInstr<ir::instr::BinaryOperationInstr>(
      ir::instr::TypeId::U64, ir::instr::BinaryOperationType::ADD, or_1, or_1);
  auto ashr_1 = bbCast->AllocateInstr<ir::instr::BinaryOperationInstr>(
      ir::instr::TypeId::U64, ir::instr::BinaryOperationType::ASHR, castInstr_7,
      castInstr_1);
  auto ashr_2 = bbCast->AllocateInstr<ir::instr::BinaryOperationInstr>(
      ir::instr::TypeId::U64, ir::instr::BinaryOperationType::ASHR, add_3,
      ashr_1);

  auto bbPhi1 = graph.AllocateBlock();
  bbPhi1->SetPreds({bbCast});
  bbCast->SetSuccs({bbPhi1});
  auto bbPhi2 = graph.AllocateBlock();
  bbPhi2->SetPreds({bbPhi1});
  bbCast->SetSuccs({bbPhi2});

  auto bbCheckPhi = graph.AllocateBlock();
  bbCheckPhi->SetPreds({bbPhi1, bbPhi2});
  bbPhi1->SetSuccs({bbCheckPhi});
  bbPhi2->SetSuccs({bbCheckPhi});

  auto bbEnd = graph.AllocateBlock();
  bbEnd->SetPreds({bbCheckPhi});
  bbCheckPhi->SetSuccs({bbEnd});

  auto phi_1 = bbCheckPhi->AllocateInstr<ir::instr::PhiInstr>(
      ir::instr::TypeId::U64, std::vector<ir::instr::Instr *>({add_2, add_2}));
  auto phi_2 = bbCheckPhi->AllocateInstr<ir::instr::PhiInstr>(
      ir::instr::TypeId::U64,
      std::vector<ir::instr::Instr *>({ashr_1, ashr_1}));
  auto add_4 = bbEnd->AllocateInstr<ir::instr::BinaryOperationInstr>(
      ir::instr::TypeId::U64, ir::instr::BinaryOperationType::ADD, phi_1,
      phi_2);

  ir::IrDumper dumper(0);
  graph.Dump(dumper);
  dumper.Endl();
  dumper.Dump(std::cout);
  dumper.Clear();

  auto constFolding = passes::ConstantFolding();
  constFolding.Run(&graph);

  for (auto block : graph.GetBlocks()) {
    for (auto instr = block->GetFirstInstr(); instr != nullptr;
         instr = instr->GetNextInstr()) {
      RETURN_IF_NOT_OK(util::CheckTrue(instr->IsConstantInstr(),
                                       "all instrs should be constant"));
    }
  }

  graph.Dump(dumper);
  dumper.Endl();
  dumper.Dump(std::cout);
  dumper.Clear();
  return 0;
}