#include "graph.h"
#include "basicBlock.h"
#include "instructions/instr.h"

#ifndef LIVENESS_ANALYZER_H
#define LIVENESS_ANALYZER_H

namespace analyzer {
class LivenessAnalyzer {
public:
    explicit LivenessAnalyzer(ir::MethodGraph &graph) : graph_(graph) {}
    void CalculateLiveRanges();

private:
    void InitLiveLinForInstr();

    ir::MethodGraph &graph_;
};
} // namespace analyzer

#endif