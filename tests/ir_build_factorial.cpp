#include "basicBlock.h"
#include "graph.h"
#include "instructions/constantInstr.h"
#include "instructions/instr.h"
#include "instructions/phiInstr.h"
#include <iostream>

int main() {
  auto Graph = ir::MethodGraph("factorial", 0);
  auto bbStart = Graph.AllocateBlock();
  bbStart->AllocateInstr<ir::instr::ConstantInstr>(ir::instr::TypeId::U32, 42);
  bbStart->AllocateInstr<ir::instr::ConstantInstr>(ir::instr::TypeId::U32, 42);
  bbStart->AllocateInstr<ir::instr::ConstantInstr>(ir::instr::TypeId::U32, 42);
  bbStart->AllocateInstr<ir::instr::ConstantInstr>(ir::instr::TypeId::U32, 42);
  bbStart->AllocateInstr<ir::instr::ConstantInstr>(ir::instr::TypeId::U32, 42);
  auto instr = bbStart->GetFirstInstr();
  while (instr != nullptr) {
    std::cout << instr->GetInstrId() << std::endl;
    instr = instr->GetNextInstr();
  }
  return 0;
}