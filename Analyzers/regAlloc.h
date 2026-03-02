#ifndef REG_ALLOC_H
#define REG_ALLOC_H

namespace ir {
class MethodGraph;
}

namespace analyzer {
class RegAlloc {
public:
    RegAlloc(ir::MethodGraph *methodGraph) : graph_(methodGraph) {};

    void Allocate(ir::MethodGraph *methodGraph);

private:
    ir::MethodGraph *graph_;
};
} // namespace analyzer

#endif