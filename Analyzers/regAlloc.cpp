#include "regAlloc.h"
#include "graph.h"
#include <cstdint>
#include <list>
#include <unordered_map>
#include <vector>

namespace analyzer {

void RegAlloc::Allocate(ir::MethodGraph *methodGraph) {
    std::vector<ir::MemPlaceInfo> stackSlots;
    std::unordered_map<uint8_t, std::list<const ir::MemPlaceInfo *>> freeStackSlots;
    std::unordered_map<uint8_t, std::list<const ir::MemPlaceInfo *>> freeRegisters;

    for (auto &memPlaceInfo : methodGraph->GetRegistersInfo()) {
        freeRegisters[memPlaceInfo.GetMaxSize()].push_back(&memPlaceInfo);
    }
}

}