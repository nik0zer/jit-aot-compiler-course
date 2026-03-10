#include "dominatorAnalyzer.h"
#include "irDumper.h"
#include "linearOrderAnalyzer.h"
#include "livenessAnalyzer.h"
#include "loopAnalyzer.h"
#include "regAlloc.h"
#include "test_utils/util.h"
#include <sourceIrBuilder.h>
#include <sstream>
using namespace std::literals;

int main(int argc, char **argv) {
  auto builder = IrBuilder::SourceIrBuilder(argv[1]);
  std::stringstream ss;
  auto graph = builder.Build(ss);

  util::CheckTrue(graph != nullptr, "Should have built graph");
  std::cout << ss.str();

  std::vector<ir::MemPlaceInfo> registersInfo{};
  registersInfo.reserve(4);
  registersInfo.emplace_back("r0", true, false, 64, 0);
  registersInfo.emplace_back("r1", true, false, 64, 1);
  registersInfo.emplace_back("r2", true, false, 64, 2);
  registersInfo.emplace_back("f0", true, true, 64, 3);
  graph->SetRegistersInfo(std::move(registersInfo));

  {
    ir::IrDumper dumper(0);
    graph->Dump(dumper);
    dumper.Dump(std::cout);
    dumper.Endl();
    dumper.Clear();

    analyzer::DominatorAnalyzer dominatorAnalyzer(*graph);
    analyzer::LoopAnalyzer loopAnalyzer(*graph, dominatorAnalyzer);

    dominatorAnalyzer.FindDominators(graph->GetBlocks()[0]);
    loopAnalyzer.FindLoops(graph->GetBlocks()[0], true);

    analyzer::LinearOrderAnalyzer linearOrderAnalyzer(*graph);
    linearOrderAnalyzer.CreateLinearOrder(graph->GetBlocks()[0]);
    linearOrderAnalyzer.Dump(dumper);
    dumper.Endl();
    dumper.Dump(std::cout);
    dumper.Clear();

    analyzer::LivenessAnalyzer livenessAnalyzer(*graph);
    livenessAnalyzer.CalculateLiveRanges();
    analyzer::RegAlloc regAlloc(*graph);
    regAlloc.Allocate();
    graph->Dump(dumper, true);
    dumper.Dump(std::cout);
    dumper.Endl();
    dumper.Clear();
  }

  delete graph;
  return 0;
}