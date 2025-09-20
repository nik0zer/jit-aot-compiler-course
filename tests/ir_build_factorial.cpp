#include "basicBlock.h"
#include "intsructions/phiInstr.h"
#include "graph.h"
#include <iostream>

int main() {
    auto Graph = ir::MethodGraph("factorial");
    auto bbStart = Graph.AllocateBlock();
    bbStart->AllocateInstr<ir::instr::PhiInstr>();
    std::cout << bbStart->GetInstrs().size() << std::endl;
    return 0;
}