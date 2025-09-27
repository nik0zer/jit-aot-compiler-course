#include "basicBlock.h"
#include "graph.h"
#include "intsructions/phiInstr.h"
#include <iostream>

int main() {
  auto Graph = ir::MethodGraph("factorial");
  auto bbStart = Graph.AllocateBlock();
  bbStart->AllocateInstr<ir::instr::PhiInstr>();
  std::cout << bbStart->GetInstrs().size() << std::endl;
  return 0;
}