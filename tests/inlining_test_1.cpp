#include "dominatorAnalyzer.h"
#include "inlinePass.h"
#include "irDumper.h"
#include "linearOrderAnalyzer.h"
#include "livenessAnalyzer.h"
#include "loopAnalyzer.h"
#include "regAlloc.h"
#include "test_utils/util.h"
#include <filesystem>
#include <fstream>
#include <map>
#include <ostream>
#include <sourceIrBuilder.h>
#include <sstream>
#include <unordered_map>

using namespace std::literals;

int main(int argc, char **argv) {
  std::filesystem::path path(argv[1]);
  std::vector<std::filesystem::path> methodFiles;
  std::ifstream file(path, std::ios::in);
  util::CheckTrue(file.is_open(), "Should have opened file");
  std::string line;
  while (std::getline(file, line)) {
    methodFiles.push_back(path.parent_path() / line);
  }

  std::map<std::string, ir::MethodGraph *> graphs;

  for (auto &file : methodFiles) {
    auto builder = IrBuilder::SourceIrBuilder(file.string());
    std::stringstream ss;
    auto graph = builder.Build(ss);
    util::CheckTrue(graph != nullptr, "Should have built graph");
    std::cout << ss.str();
    graphs[file.filename().string()] = graph;
  }

  for (auto &[_, graph] : graphs) {
    ir::IrDumper dumper(0);
    graph->Dump(dumper);
    dumper.Endl();
    dumper.Dump(std::cout);
  }

  std::cout << std::endl << std::endl;

  auto getGraph = [&](const std::string &name) {
    auto graphPath = (path.parent_path() / (name + ".fa")).string();
    auto builder = IrBuilder::SourceIrBuilder(graphPath);
    std::stringstream ss;
    auto graph = builder.Build(ss);
    util::CheckTrue(graph != nullptr, "Should have built graph");
    std::cout << ss.str();
    return graph;
  };

  auto inlinePass = passes::InlinePass(getGraph);
  inlinePass.Run(graphs.begin()->second);
  ir::IrDumper dumper(0);
  graphs.begin()->second->Dump(dumper);
  dumper.Endl();
  dumper.Dump(std::cout);

  for (auto &[_, graph] : graphs) {
    delete graph;
  }

  return 0;
}