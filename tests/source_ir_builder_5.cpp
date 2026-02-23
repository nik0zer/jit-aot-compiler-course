#include <sourceIrBuilder.h>
#include <sstream>
#include "irDumper.h"
#include "test_utils/util.h"

int main(int argc, char **argv) {
    auto builder = IrBuilder::SourceIrBuilder(argv[1]);
    std::stringstream ss;
    auto graph = builder.Build(ss);

    util::CheckTrue(graph != nullptr, "Should have built graph");
    std::cout << ss.str();

    ir::IrDumper dumper(0);
    graph->Dump(dumper);
    dumper.Dump(std::cout);
    delete graph;
    return 0;
}