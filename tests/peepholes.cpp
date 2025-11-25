#include "peepholes.h"
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
  auto graph = ir::MethodGraph("TestOptimizerPasses_2", 0);
  auto bbStart = graph.AllocateBlock();

  auto bbCheckAdd = graph.AllocateBlock();
  bbCheckAdd->SetPreds({bbStart});
  bbStart->SetSuccs({bbCheckAdd});

  auto bbCheckOr = graph.AllocateBlock();
  bbCheckOr->SetPreds({bbStart});
  bbCheckAdd->SetSuccs({bbCheckOr});

  auto bbCheckAshr = graph.AllocateBlock();
  bbCheckAshr->SetPreds({bbStart});
  bbCheckOr->SetSuccs({bbCheckAshr});

  auto bbEnd = graph.AllocateBlock();
  bbEnd->SetPreds({bbCheckAshr});
  bbCheckAshr->SetSuccs({bbEnd});

  auto paramA =
      bbStart->AllocateInstr<ir::instr::ParamInstr>(ir::instr::TypeId::I32, 0);
  auto paramB =
      bbStart->AllocateInstr<ir::instr::ParamInstr>(ir::instr::TypeId::U32, 1);

  auto constZero = bbCheckAdd->AllocateInstr<ir::instr::ConstantInstr>(
      ir::instr::TypeId::I32, 0);
  auto constOne = bbCheckAdd->AllocateInstr<ir::instr::ConstantInstr>(
      ir::instr::TypeId::I32, -1);
  auto constMax = bbCheckAdd->AllocateInstr<ir::instr::ConstantInstr>(
      ir::instr::TypeId::U32, std::numeric_limits<uint32_t>::max());
  auto constMaxShift = bbCheckAdd->AllocateInstr<ir::instr::ConstantInstr>(
      ir::instr::TypeId::I32, 32);

  auto addPeephole_1 =
      bbCheckAdd->AllocateInstr<ir::instr::BinaryOperationInstr>(
          ir::instr::TypeId::I32, ir::instr::BinaryOperationType::ADD, paramA,
          constZero);
  auto usePeephole_1 = bbCheckAdd->AllocateInstr<ir::instr::CastInstr>(
      ir::instr::TypeId::I64, addPeephole_1);
  auto addPeephole_2 =
      bbCheckAdd->AllocateInstr<ir::instr::BinaryOperationInstr>(
          ir::instr::TypeId::I32, ir::instr::BinaryOperationType::ADD,
          constZero, paramA);
  auto usePeephole_2 = bbCheckAdd->AllocateInstr<ir::instr::CastInstr>(
      ir::instr::TypeId::I64, addPeephole_2);
  auto subParams = bbCheckAdd->AllocateInstr<ir::instr::BinaryOperationInstr>(
      ir::instr::TypeId::I32, ir::instr::BinaryOperationType::SUB, paramA,
      paramB);
  auto addPeephole_3 =
      bbCheckAdd->AllocateInstr<ir::instr::BinaryOperationInstr>(
          ir::instr::TypeId::I32, ir::instr::BinaryOperationType::ADD,
          subParams, paramB);
  auto usePeephole_3 = bbCheckAdd->AllocateInstr<ir::instr::CastInstr>(
      ir::instr::TypeId::I64, addPeephole_3);
  auto addPeephole_4 =
      bbCheckAdd->AllocateInstr<ir::instr::BinaryOperationInstr>(
          ir::instr::TypeId::I32, ir::instr::BinaryOperationType::ADD, paramB,
          subParams);
  auto usePeephole_4 = bbCheckAdd->AllocateInstr<ir::instr::CastInstr>(
      ir::instr::TypeId::I64, addPeephole_4);

  auto orPeephole_1 = bbCheckOr->AllocateInstr<ir::instr::BinaryOperationInstr>(
      ir::instr::TypeId::I32, ir::instr::BinaryOperationType::OR, paramA,
      constZero);
  auto useOrPeephole_1 = bbCheckOr->AllocateInstr<ir::instr::CastInstr>(
      ir::instr::TypeId::I64, orPeephole_1);
  auto orPeephole_2 = bbCheckOr->AllocateInstr<ir::instr::BinaryOperationInstr>(
      ir::instr::TypeId::I32, ir::instr::BinaryOperationType::OR, constZero,
      paramA);
  auto useOrPeephole_2 = bbCheckOr->AllocateInstr<ir::instr::CastInstr>(
      ir::instr::TypeId::I64, orPeephole_2);
  auto orPeephole_3 = bbCheckOr->AllocateInstr<ir::instr::BinaryOperationInstr>(
      ir::instr::TypeId::I32, ir::instr::BinaryOperationType::OR, paramA,
      paramA);
  auto useOrPeephole_3 = bbCheckOr->AllocateInstr<ir::instr::CastInstr>(
      ir::instr::TypeId::I64, orPeephole_3);
  auto orPeephole_4 = bbCheckOr->AllocateInstr<ir::instr::BinaryOperationInstr>(
      ir::instr::TypeId::I32, ir::instr::BinaryOperationType::OR, paramA,
      constOne);
  auto useOrPeephole_4 = bbCheckOr->AllocateInstr<ir::instr::CastInstr>(
      ir::instr::TypeId::I64, orPeephole_4);
  auto orPeephole_5 = bbCheckOr->AllocateInstr<ir::instr::BinaryOperationInstr>(
      ir::instr::TypeId::I32, ir::instr::BinaryOperationType::OR, constOne,
      paramA);
  auto useOrPeephole_5 = bbCheckOr->AllocateInstr<ir::instr::CastInstr>(
      ir::instr::TypeId::I64, orPeephole_5);
  auto orPeephole_6 = bbCheckOr->AllocateInstr<ir::instr::BinaryOperationInstr>(
      ir::instr::TypeId::U32, ir::instr::BinaryOperationType::OR, paramB,
      constMax);
  auto useOrPeephole_6 = bbCheckOr->AllocateInstr<ir::instr::CastInstr>(
      ir::instr::TypeId::I64, orPeephole_6);
  auto orPeephole_7 = bbCheckOr->AllocateInstr<ir::instr::BinaryOperationInstr>(
      ir::instr::TypeId::U32, ir::instr::BinaryOperationType::OR, constMax,
      paramB);
  auto useOrPeephole_7 = bbCheckOr->AllocateInstr<ir::instr::CastInstr>(
      ir::instr::TypeId::I64, orPeephole_7);

  auto ashrPeephole_1 =
      bbCheckAshr->AllocateInstr<ir::instr::BinaryOperationInstr>(
          ir::instr::TypeId::I32, ir::instr::BinaryOperationType::ASHR, paramA,
          constMaxShift);
  auto useAshrPeephole_1 = bbCheckAshr->AllocateInstr<ir::instr::CastInstr>(
      ir::instr::TypeId::I64, ashrPeephole_1);
  auto ashrPeephole_2 =
      bbCheckAshr->AllocateInstr<ir::instr::BinaryOperationInstr>(
          ir::instr::TypeId::I32, ir::instr::BinaryOperationType::ASHR, paramA,
          constZero);
  auto useAshrPeephole_2 = bbCheckAshr->AllocateInstr<ir::instr::CastInstr>(
      ir::instr::TypeId::I64, ashrPeephole_2);

  ir::IrDumper dumper(0);
  graph.Dump(dumper);
  dumper.Endl();
  dumper.Dump(std::cout);
  dumper.Clear();

  auto peepholes = passes::PeepHoles();
  peepholes.Run(&graph);

  for (auto block : graph.GetBlocks()) {
    for (auto instr = block->GetFirstInstr(); instr != nullptr;
         instr = instr->GetNextInstr()) {
      RETURN_IF_NOT_OK(util::CheckTrue(
          !instr->IsBinaryOperationInstr() ||
              (instr->IsBinaryOperationInstr() &&
               instr->AsBinaryOperationInstr()->GetOperation() ==
                   ir::instr::BinaryOperationType::SUB),
          "BinaryOperationInstr(ADD, OR, ASHR) should be optimized away"));
    }
  }

  graph.Dump(dumper);
  dumper.Endl();
  dumper.Dump(std::cout);
  dumper.Clear();
  return 0;
}