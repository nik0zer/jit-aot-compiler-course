#include "basicBlock.h"
#include "graph.h"
#include "instructions/constantInstr.h"
#include "instructions/instr.h"
#include "instructions/phiInstr.h"
#include <iostream>
#include "instructions/binaryOperationInstr.h"
#include "irDumper.h"
#include "instructions/callInstr.h"
#include "instructions/castInstr.h"
#include "instructions/constantInstr.h"

int main() {
  auto Graph = ir::MethodGraph("factorial", 0);
  auto bbStart = Graph.AllocateBlock();
  auto instr = bbStart->AllocateInstr<ir::instr::ConstantInstr>(ir::instr::TypeId::U32, 42);
  instr = bbStart->AllocateInstr<ir::instr::ConstantInstr>(ir::instr::TypeId::U32, 42);
  instr = bbStart->AllocateInstr<ir::instr::PhiInstr>(ir::instr::TypeId::U32, std::vector<ir::instr::Instr *>({instr}));
  auto instr1 = bbStart->AllocateInstr<ir::instr::ConstantInstr>(ir::instr::TypeId::U32, 42);
  auto instr2 = bbStart->AllocateInstr<ir::instr::ConstantInstr>(ir::instr::TypeId::U32, 4242);
  instr = bbStart->AllocateInstr<ir::instr::BinaryOperationInstr>(ir::instr::TypeId::U32, ir::instr::BinaryOperation::ADD, instr1, instr2);
  instr = bbStart->AllocateInstr<ir::instr::CallInstr>(ir::instr::TypeId::U32, 15, std::vector<ir::instr::Instr *>({instr, instr1, instr2}));
  instr = bbStart->AllocateInstr<ir::instr::CastInstr>(ir::instr::TypeId::U64, instr);
  instr = bbStart->AllocateInstr<ir::instr::PhiInstr>(ir::instr::TypeId::U32, std::vector<ir::instr::Instr *>({instr}));
  instr = bbStart->AllocateInstr<ir::instr::ConstantInstr>(ir::instr::TypeId::U32, 42);
  auto firstInstr = bbStart->GetFirstInstr();
  ir::IrDumper dumper(0);
  bbStart->Dump(dumper);
  dumper.Dump(std::cout);
  dumper.Clear();
  return 0;
}