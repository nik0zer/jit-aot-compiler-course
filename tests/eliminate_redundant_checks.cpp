#include "CheckElimination.h"
#include "irDumper.h"
#include "test_utils/util.h"
#include <sourceIrBuilder.h>
#include <sstream>

int main(int argc, char **argv) {
  auto builder = IrBuilder::SourceIrBuilder(argv[1]);
  std::stringstream ss;
  auto graph = builder.Build(ss);

  std::cout << ss.str();
  util::CheckTrue(graph != nullptr, "Should have built graph");

  {
    passes::CheckElimination checkElimination(true);
    checkElimination.Run(graph);

    ir::IrDumper dumper(0);
    graph->Dump(dumper);
    dumper.Dump(std::cout);
    dumper.Endl();
    dumper.Clear();
  }

  delete graph;
  return 0;
}