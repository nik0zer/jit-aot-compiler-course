#include "instr.h"
#include "basicBlock.h"
#include "graph.h"
#include "irDumper.h"

namespace ir::instr {

void Instr::Dump(IrDumper &dumper, bool dumpLiveness) {
  if (dumpLiveness) {
    dumper.Add("[ live: ");
    dumper.Add(liveRange_.GetLive());
    dumper.Add(" lin: ");
    dumper.Add(liveRange_.GetLin());
    dumper.Add(" range: [");
    dumper.Add(liveRange_.GetRange().first);
    dumper.Add(":");
    dumper.Add(liveRange_.GetRange().second);
    dumper.Add("] intervals: [");
    for (auto interval : liveRange_.GetIntervals()) {
      dumper.Add(" [");
      dumper.Add(interval.first);
      dumper.Add(":");
      dumper.Add(interval.second);
      dumper.Add("] ");
    }
    dumper.Add("] uses: [");
    for (auto use : liveRange_.GetUses()) {
      dumper.Add(use);
      if (use != liveRange_.GetUses().back()) {
        dumper.Add(", ");
      }
    }
    dumper.Add("] ] ");
    if (!regAllocInfo_.GetAllocationRanges().empty()) {
      dumper.Add("[ MemPlaces: ");
      for (auto [range, memPlace] : regAllocInfo_.GetAllocationRanges()) {
        dumper.Add(memPlace->GetName());
        dumper.Add(":[");
        dumper.Add(range.first);
        dumper.Add(":");
        dumper.Add(range.second);
        dumper.Add("]");
        if (memPlace != regAllocInfo_.GetAllocationRanges().back().second) {
          dumper.Add(", ");
        }
      }
      dumper.Add(" ] ");
    }
  }
  dumper.Add(id_);
  dumper.Add(".");
  dumper.Add(TypeIdToString(type_).data());
}

} // namespace ir::instr