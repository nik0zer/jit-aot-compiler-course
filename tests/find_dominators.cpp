#include "basicBlock.h"
#include "dominatorAnalyzer.h"
#include "graph.h"
#include "instructions/instr.h"
#include "irDumper.h"
#include "macro.h"
#include "test_utils/util.h"
#include <cstddef>
#include <iostream>

int main() {
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
    domAnalyzer.DumpDominators(dumper, bbA);
    dumper.Endl();
    analyzer::DominatorTree domTree(graph, bbA);
    domTree.Dump(dumper);
    dumper.Endl();
    dumper.Endl();
    dumper.Dump(std::cout);
    dumper.Clear();

    util::CheckEqual<ir::BasicBlock *>(bbA->GetIDominator(), nullptr,
                                       "bbA->GetIDominator() == nullptr");
    util::CheckEqual<ir::BasicBlock *>(bbB->GetIDominator(), bbA,
                                       "bbB->GetIDominator() == bbA");
    util::CheckEqual<ir::BasicBlock *>(bbC->GetIDominator(), bbB,
                                       "bbC->GetIDominator() == bbB");
    util::CheckEqual<ir::BasicBlock *>(bbD->GetIDominator(), bbB,
                                       "bbD->GetIDominator() == bbB");
    util::CheckEqual<ir::BasicBlock *>(bbE->GetIDominator(), bbF,
                                       "bbE->GetIDominator() == bbF");
    util::CheckEqual<ir::BasicBlock *>(bbF->GetIDominator(), bbB,
                                       "bbF->GetIDominator() == bbB");
    util::CheckEqual<ir::BasicBlock *>(bbG->GetIDominator(), bbF,
                                       "bbG->GetIDominator() == bbF");
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
    domAnalyzer.DumpDominators(dumper, bbA);
    dumper.Endl();
    analyzer::DominatorTree domTree(graph, bbA);
    domTree.Dump(dumper);
    dumper.Endl();
    dumper.Endl();
    dumper.Dump(std::cout);
    dumper.Clear();

    util::CheckEqual<ir::BasicBlock *>(bbA->GetIDominator(), nullptr,
                                       "bbA->GetIDominator() == nullptr");
    util::CheckEqual<ir::BasicBlock *>(bbB->GetIDominator(), bbA,
                                       "bbB->GetIDominator() == bbA");
    util::CheckEqual<ir::BasicBlock *>(bbC->GetIDominator(), bbB,
                                       "bbC->GetIDominator() == bbB");
    util::CheckEqual<ir::BasicBlock *>(bbD->GetIDominator(), bbC,
                                       "bbD->GetIDominator() == bbC");
    util::CheckEqual<ir::BasicBlock *>(bbE->GetIDominator(), bbD,
                                       "bbE->GetIDominator() == bbD");
    util::CheckEqual<ir::BasicBlock *>(bbF->GetIDominator(), bbE,
                                       "bbF->GetIDominator() == bbE");
    util::CheckEqual<ir::BasicBlock *>(bbG->GetIDominator(), bbF,
                                       "bbG->GetIDominator() == bbF");
    util::CheckEqual<ir::BasicBlock *>(bbH->GetIDominator(), bbG,
                                       "bbH->GetIDominator() == bbG");
    util::CheckEqual<ir::BasicBlock *>(bbI->GetIDominator(), bbG,
                                       "bbI->GetIDominator() == bbG");
    util::CheckEqual<ir::BasicBlock *>(bbK->GetIDominator(), bbI,
                                       "bbK->GetIDominator() == bbI");
    util::CheckEqual<ir::BasicBlock *>(bbJ->GetIDominator(), bbB,
                                       "bbJ->GetIDominator() == bbB");
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
    domAnalyzer.DumpDominators(dumper, bbA);
    dumper.Endl();
    analyzer::DominatorTree domTree(graph, bbA);
    domTree.Dump(dumper);
    dumper.Endl();
    dumper.Endl();
    dumper.Dump(std::cout);
    dumper.Clear();

    util::CheckEqual<ir::BasicBlock *>(bbA->GetIDominator(), nullptr,
                                       "bbA->GetIDominator() == nullptr");
    util::CheckEqual<ir::BasicBlock *>(bbB->GetIDominator(), bbA,
                                       "bbB->GetIDominator() == bbA");
    util::CheckEqual<ir::BasicBlock *>(bbC->GetIDominator(), bbB,
                                       "bbC->GetIDominator() == bbB");
    util::CheckEqual<ir::BasicBlock *>(bbD->GetIDominator(), bbB,
                                       "bbD->GetIDominator() == bbB");
    util::CheckEqual<ir::BasicBlock *>(bbE->GetIDominator(), bbB,
                                       "bbE->GetIDominator() == bbB");
    util::CheckEqual<ir::BasicBlock *>(bbF->GetIDominator(), bbE,
                                       "bbF->GetIDominator() == bbE");
    util::CheckEqual<ir::BasicBlock *>(bbG->GetIDominator(), bbB,
                                       "bbG->GetIDominator() == bbB");
    util::CheckEqual<ir::BasicBlock *>(bbH->GetIDominator(), bbF,
                                       "bbH->GetIDominator() == bbF");
    util::CheckEqual<ir::BasicBlock *>(bbI->GetIDominator(), bbB,
                                       "bbI->GetIDominator() == bbB");
  }
  return 0;
}