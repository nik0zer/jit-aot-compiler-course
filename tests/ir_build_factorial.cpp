#include "basicBlock.h"
#include "graph.h"
#include "instructions/binaryOperationInstr.h"
#include "instructions/callInstr.h"
#include "instructions/castInstr.h"
#include "instructions/constantInstr.h"
#include "instructions/ifInstr.h"
#include "instructions/instr.h"
#include "instructions/paramInstr.h"
#include "instructions/phiInstr.h"
#include "instructions/returnInstr.h"
#include "irDumper.h"
#include <iostream>

int main() {
  auto graph = ir::MethodGraph("factorial", 0);
  auto bbStart = graph.AllocateBlock();
  auto param =
      bbStart->AllocateInstr<ir::instr::ParamInstr>(ir::instr::TypeId::U32, 0);
  auto bbConstants = graph.AllocateBlock();
  bbConstants->SetPreds({bbStart});
  bbStart->SetSuccs({bbConstants});
  auto v0_const = bbConstants->AllocateInstr<ir::instr::ConstantInstr>(
      ir::instr::TypeId::U64, 1);
  auto const1 = bbConstants->AllocateInstr<ir::instr::ConstantInstr>(
      ir::instr::TypeId::U64, 1);
  auto v1_const = bbConstants->AllocateInstr<ir::instr::ConstantInstr>(
      ir::instr::TypeId::U64, 2);
  auto v2_const = bbConstants->AllocateInstr<ir::instr::CastInstr>(
      ir::instr::TypeId::U64, param);

  auto bbLoopCheck = graph.AllocateBlock();
  auto bbLoopBody = graph.AllocateBlock();
  auto bbRet = graph.AllocateBlock();
  bbConstants->SetSuccs({bbLoopCheck});
  bbLoopCheck->SetPreds({bbConstants, bbLoopBody});
  bbLoopCheck->SetSuccs({bbLoopBody, bbRet});
  bbLoopBody->SetPreds({bbLoopCheck});
  bbLoopBody->SetSuccs({bbLoopCheck});
  bbRet->SetPreds({bbLoopCheck});

  auto phi_v0 = bbLoopCheck->AllocateInstr<ir::instr::PhiInstr>(
      ir::instr::TypeId::U64,
      std::vector<ir::instr::Instr *>({v0_const, nullptr}));
  auto phi_v1 = bbLoopCheck->AllocateInstr<ir::instr::PhiInstr>(
      ir::instr::TypeId::U64,
      std::vector<ir::instr::Instr *>({v1_const, nullptr}));
  bbLoopCheck->AllocateInstr<ir::instr::IfInstr>(phi_v0, v2_const,
                                                 ir::instr::IfType::LE);

  auto mul = bbLoopBody->AllocateInstr<ir::instr::BinaryOperationInstr>(
      ir::instr::TypeId::U64, ir::instr::BinaryOperationType::MUL, phi_v0,
      phi_v1);
  auto add = bbLoopBody->AllocateInstr<ir::instr::BinaryOperationInstr>(
      ir::instr::TypeId::U64, ir::instr::BinaryOperationType::ADD, phi_v1,
      const1);

  phi_v0->ReplaceInput(mul, phi_v0->GetInputs().size() - 1);
  phi_v1->ReplaceInput(add, phi_v1->GetInputs().size() - 1);

  bbRet->AllocateInstr<ir::instr::ReturnInstr>(ir::instr::TypeId::U64, phi_v0);

  auto firstInstr = bbStart->GetFirstInstr();
  ir::IrDumper dumper(0);
  graph.Dump(dumper);
  dumper.Dump(std::cout);
  dumper.Clear();
  return 0;
}