#include "basicBlock.h"
#include "graph.h"
#include "intsructions/phiInstr.h"
#include <iostream>
#include "intsructions/constantInstr.h"

int main() {
  auto Graph = ir::MethodGraph("factorial", 0);
  auto bbStart = Graph.AllocateBlock();
  bbStart->AllocateInstr<ir::instr::PhiInstr>(ir::instr::TypeId::U32);
  std::cout << bbStart->GetInstrs().size() << std::endl;
  return 0;
}