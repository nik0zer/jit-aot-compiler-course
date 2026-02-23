#include <sourceIrBuilder.h>
#include <sstream>
#include "dominatorAnalyzer.h"
#include "irDumper.h"
#include "loopAnalyzer.h"
#include "linearOrderAnalyzer.h"
#include "test_utils/util.h"

int main(int argc, char **argv) {
    auto builder = IrBuilder::SourceIrBuilder(argv[1]);
    std::stringstream ss;
    auto graph = builder.Build(ss);

    util::CheckTrue(graph != nullptr, "Should have built graph");
    std::cout << ss.str();

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
        dumper.Dump(std::cout);
        dumper.Endl();
        dumper.Clear();
    }


    delete graph;
    return 0;
}