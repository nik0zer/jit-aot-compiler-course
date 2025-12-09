#include "basicBlock.h"
#include "dominatorAnalyzer.h"
#include "graph.h"
#include "instructions/instr.h"
#include "irDumper.h"
#include "loopAnalyzer.h"
#include "macro.h"
#include "test_utils/util.h"
#include <cstddef>
#include <iostream>

int main() {
  {
    auto graph = ir::MethodGraph("Example 0.1", 0);
    auto bbA = graph.AllocateBlock(); // 0
    auto bbB = graph.AllocateBlock(); // 1
    auto bbC = graph.AllocateBlock(); // 2
    auto bbD = graph.AllocateBlock(); // 3
    auto bbE = graph.AllocateBlock(); // 4

    bbA->SetSuccs({bbB});
    bbB->SetSuccs({bbC, bbD});
    bbD->SetSuccs({bbE});
    bbE->SetSuccs({bbB});

    bbB->SetPreds({bbA, bbE});
    bbC->SetPreds({bbB});
    bbD->SetPreds({bbB});
    bbE->SetPreds({bbD});

    ir::IrDumper dumper(0);
    analyzer::DominatorAnalyzer domAnalyzer(graph);
    graph.Dump(dumper);
    dumper.Dump(std::cout);
    dumper.Clear();
    dumper.Endl();
    domAnalyzer.DumpRPO(dumper, bbA);
    dumper.Dump(std::cout);
    dumper.Clear();
    dumper.Endl();
    dumper.Endl();
    domAnalyzer.FindDominators(bbA);
    analyzer::LoopAnalyzer loopAnalyzer(graph, domAnalyzer);
    loopAnalyzer.FindLoops(bbA, true);
    loopAnalyzer.Dump(dumper);
    dumper.Dump(std::cout);
    dumper.Clear();
    dumper.Endl();
  }

  {
    auto graph = ir::MethodGraph("Example 0.2", 0);
    auto bbA = graph.AllocateBlock(); // 0
    auto bbB = graph.AllocateBlock(); // 1
    auto bbC = graph.AllocateBlock(); // 2
    auto bbD = graph.AllocateBlock(); // 3
    auto bbE = graph.AllocateBlock(); // 4
    auto bbF = graph.AllocateBlock(); // 5

    bbA->SetSuccs({bbB});
    bbB->SetSuccs({bbC});
    bbC->SetSuccs({bbD, bbF});
    bbD->SetSuccs({bbE});
    bbE->SetSuccs({bbB});

    bbB->SetPreds({bbA, bbE});
    bbC->SetPreds({bbB});
    bbD->SetPreds({bbC});
    bbE->SetPreds({bbD});
    bbF->SetPreds({bbC, bbD});

    ir::IrDumper dumper(0);
    analyzer::DominatorAnalyzer domAnalyzer(graph);
    graph.Dump(dumper);
    dumper.Dump(std::cout);
    dumper.Clear();
    dumper.Endl();
    domAnalyzer.DumpRPO(dumper, bbA);
    dumper.Dump(std::cout);
    dumper.Clear();
    dumper.Endl();
    dumper.Endl();
    domAnalyzer.FindDominators(bbA);
    analyzer::LoopAnalyzer loopAnalyzer(graph, domAnalyzer);
    loopAnalyzer.FindLoops(bbA, true);
    loopAnalyzer.Dump(dumper);
    dumper.Dump(std::cout);
    dumper.Clear();
    dumper.Endl();
  }

  {
    auto graph = ir::MethodGraph("Example 0.3", 0);
    auto bbA = graph.AllocateBlock(); // 0
    auto bbB = graph.AllocateBlock(); // 1
    auto bbC = graph.AllocateBlock(); // 2
    auto bbD = graph.AllocateBlock(); // 3
    auto bbE = graph.AllocateBlock(); // 4
    auto bbF = graph.AllocateBlock(); // 5
    auto bbG = graph.AllocateBlock(); // 6
    auto bbH = graph.AllocateBlock(); // 7

    bbA->SetSuccs({bbB});
    bbB->SetSuccs({bbC, bbD});
    bbC->SetSuccs({bbE, bbH});
    bbD->SetSuccs({bbE});
    bbE->SetSuccs({bbF});
    bbF->SetSuccs({bbG, bbB});
    bbG->SetSuccs({bbA});

    bbA->SetPreds({bbG});
    bbB->SetPreds({bbA, bbF});
    bbC->SetPreds({bbB});
    bbD->SetPreds({bbB});
    bbE->SetPreds({bbC, bbD});
    bbF->SetPreds({bbE});
    bbG->SetPreds({bbF});
    bbH->SetPreds({bbC});

    ir::IrDumper dumper(0);
    analyzer::DominatorAnalyzer domAnalyzer(graph);
    graph.Dump(dumper);
    dumper.Dump(std::cout);
    dumper.Clear();
    dumper.Endl();
    domAnalyzer.DumpRPO(dumper, bbA);
    dumper.Dump(std::cout);
    dumper.Clear();
    dumper.Endl();
    dumper.Endl();
    domAnalyzer.FindDominators(bbA);
    analyzer::LoopAnalyzer loopAnalyzer(graph, domAnalyzer);
    loopAnalyzer.FindLoops(bbA, true);
    loopAnalyzer.Dump(dumper);
    dumper.Dump(std::cout);
    dumper.Clear();
    dumper.Endl();
  }

  {
    auto graph = ir::MethodGraph("Example 1", 0);
    auto bbA = graph.AllocateBlock(); // 0
    auto bbB = graph.AllocateBlock(); // 1
    auto bbC = graph.AllocateBlock(); // 2
    auto bbD = graph.AllocateBlock(); // 3
    auto bbE = graph.AllocateBlock(); // 4
    auto bbF = graph.AllocateBlock(); // 5
    auto bbG = graph.AllocateBlock(); // 6

    bbA->SetSuccs({bbB});
    bbB->SetSuccs({bbC, bbF});
    bbC->SetSuccs({bbD});
    bbF->SetSuccs({bbE, bbG});
    bbE->SetSuccs({bbD});
    bbG->SetSuccs({bbD});

    bbB->SetPreds({bbA});
    bbC->SetPreds({bbB});
    bbD->SetPreds({bbC, bbE, bbG});
    bbF->SetPreds({bbB});
    bbE->SetPreds({bbF});
    bbG->SetPreds({bbF});

    ir::IrDumper dumper(0);
    analyzer::DominatorAnalyzer domAnalyzer(graph);
    graph.Dump(dumper);
    dumper.Dump(std::cout);
    dumper.Clear();
    dumper.Endl();
    domAnalyzer.DumpRPO(dumper, bbA);
    dumper.Dump(std::cout);
    dumper.Clear();
    dumper.Endl();
    dumper.Endl();
    domAnalyzer.FindDominators(bbA);
    analyzer::LoopAnalyzer loopAnalyzer(graph, domAnalyzer);
    loopAnalyzer.FindLoops(bbA, true);
    loopAnalyzer.Dump(dumper);
    dumper.Dump(std::cout);
    dumper.Clear();
    dumper.Endl();
  }

  {
    auto graph = ir::MethodGraph("Example 2", 0);
    auto bbA = graph.AllocateBlock(); // 0
    auto bbB = graph.AllocateBlock(); // 1
    auto bbC = graph.AllocateBlock(); // 2
    auto bbD = graph.AllocateBlock(); // 3
    auto bbE = graph.AllocateBlock(); // 4
    auto bbF = graph.AllocateBlock(); // 5
    auto bbG = graph.AllocateBlock(); // 6
    auto bbH = graph.AllocateBlock(); // 7
    auto bbI = graph.AllocateBlock(); // 8
    auto bbJ = graph.AllocateBlock(); // 9
    auto bbK = graph.AllocateBlock(); // 10

    bbA->SetSuccs({bbB});
    bbB->SetSuccs({bbC, bbJ});
    bbC->SetSuccs({bbD});
    bbD->SetSuccs({bbE, bbC});
    bbE->SetSuccs({bbF});
    bbF->SetSuccs({bbG, bbE});
    bbG->SetSuccs({bbH, bbI});
    bbH->SetSuccs({bbB});
    bbI->SetSuccs({bbK});
    bbJ->SetSuccs({bbC});

    bbB->SetPreds({bbA, bbH});
    bbC->SetPreds({bbB, bbJ});
    bbD->SetPreds({bbC});
    bbE->SetPreds({bbD, bbF});
    bbF->SetPreds({bbE});
    bbG->SetPreds({bbF});
    bbH->SetPreds({bbG});
    bbI->SetPreds({bbG});
    bbJ->SetPreds({bbB});
    bbK->SetPreds({bbI});

    ir::IrDumper dumper(0);
    analyzer::DominatorAnalyzer domAnalyzer(graph);
    graph.Dump(dumper);
    dumper.Dump(std::cout);
    dumper.Clear();
    dumper.Endl();
    domAnalyzer.DumpRPO(dumper, bbA);
    dumper.Dump(std::cout);
    dumper.Clear();
    dumper.Endl();
    dumper.Endl();
    domAnalyzer.FindDominators(bbA);
    analyzer::LoopAnalyzer loopAnalyzer(graph, domAnalyzer);
    loopAnalyzer.FindLoops(bbA, true);
    loopAnalyzer.Dump(dumper);
    dumper.Dump(std::cout);
    dumper.Clear();
    dumper.Endl();
  }

  {
    auto graph = ir::MethodGraph("Example 3", 0);
    auto bbA = graph.AllocateBlock(); // 0
    auto bbB = graph.AllocateBlock(); // 1
    auto bbC = graph.AllocateBlock(); // 2
    auto bbD = graph.AllocateBlock(); // 3
    auto bbE = graph.AllocateBlock(); // 4
    auto bbF = graph.AllocateBlock(); // 5
    auto bbG = graph.AllocateBlock(); // 6
    auto bbH = graph.AllocateBlock(); // 7
    auto bbI = graph.AllocateBlock(); // 8

    bbA->SetSuccs({bbB});
    bbB->SetSuccs({bbC, bbE});
    bbC->SetSuccs({bbD});
    bbD->SetSuccs({bbG});
    bbE->SetSuccs({bbF, bbD});
    bbF->SetSuccs({bbH, bbB});
    bbG->SetSuccs({bbI, bbC});
    bbH->SetSuccs({bbG, bbI});
    bbI->SetSuccs({nullptr});

    bbB->SetPreds({bbA, bbF});
    bbC->SetPreds({bbB, bbG});
    bbD->SetPreds({bbC, bbE});
    bbE->SetPreds({bbB});
    bbF->SetPreds({bbE});
    bbG->SetPreds({bbD, bbH});
    bbH->SetPreds({bbF});
    bbI->SetPreds({bbG, bbH});

    ir::IrDumper dumper(0);
    analyzer::DominatorAnalyzer domAnalyzer(graph);
    graph.Dump(dumper);
    dumper.Dump(std::cout);
    dumper.Clear();
    dumper.Endl();
    domAnalyzer.DumpRPO(dumper, bbA);
    dumper.Dump(std::cout);
    dumper.Clear();
    dumper.Endl();
    dumper.Endl();
    domAnalyzer.FindDominators(bbA);
    analyzer::LoopAnalyzer loopAnalyzer(graph, domAnalyzer);
    loopAnalyzer.FindLoops(bbA, true);
    loopAnalyzer.Dump(dumper);
    dumper.Dump(std::cout);
    dumper.Clear();
    dumper.Endl();
  }
  return 0;
}