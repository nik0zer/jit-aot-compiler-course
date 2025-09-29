#include "basicBlock.h"
#include "graph.h"
#include "instructions/constantInstr.h"
#include "instructions/instr.h"
#include "instructions/phiInstr.h"
#include <iostream>
#include "instructions/binaryOperationInstr.h"
#include "irDumper.h"
#include "instructions/callInstr.h"

int main() {
  auto Graph = ir::MethodGraph("factorial", 0);
  auto bbStart = Graph.AllocateBlock();
  auto instr = bbStart->AllocateInstr<ir::instr::ConstantInstr>(ir::instr::TypeId::U32, 42);
  instr = bbStart->AllocateInstr<ir::instr::ConstantInstr>(ir::instr::TypeId::U32, 42);
  instr = bbStart->AllocateInstr<ir::instr::PhiInstr>(ir::instr::TypeId::U32, std::vector<ir::instr::Instr *>({instr}));
  auto instr1 = bbStart->AllocateInstr<ir::instr::ConstantInstr>(ir::instr::TypeId::U32, 42);
  auto instr2 = bbStart->AllocateInstr<ir::instr::ConstantInstr>(ir::instr::TypeId::U32, 4242);
  instr = bbStart->AllocateInstr<ir::instr::BinaryOperationInstr>(ir::instr::TypeId::U32, ir::instr::BinaryOperation::ADD, instr1, instr2);
  ir::IrDumper dumper(0);
  instr->Dump(dumper);
  dumper.Dump(std::cout);
  std::cout << std::endl;
  dumper.Clear();
  instr = bbStart->AllocateInstr<ir::instr::CallInstr>(ir::instr::TypeId::U32, 15, std::vector<ir::instr::Instr *>({instr, instr1, instr2}));
  instr->Dump(dumper);
  dumper.Dump(std::cout);
  std::cout << std::endl;
  dumper.Clear();
  instr = bbStart->AllocateInstr<ir::instr::PhiInstr>(ir::instr::TypeId::U32, std::vector<ir::instr::Instr *>({instr}));
  instr = bbStart->AllocateInstr<ir::instr::ConstantInstr>(ir::instr::TypeId::U32, 42);
  auto firstInstr = bbStart->GetFirstInstr();
  while (firstInstr != nullptr) {
    std::cout << firstInstr->GetInstrId() << std::endl;
    std::cout << firstInstr << std::endl;
    firstInstr = firstInstr->GetNextInstr();
  }
  return 0;
}