#include "regAlloc.h"
#include "graph.h"
#include "instructions/instr.h"
#include "instructions/type.h"
#include "linearOrderAnalyzer.h"
#include "macro.h"
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <deque>
#include <list>
#include <memory>
#include <unordered_map>
#include <vector>

namespace analyzer {
enum class EventType { Use, Die, StartInterval, EndInterval };
struct AliveInstrEvent {
  AliveInstrEvent() = default;
  AliveInstrEvent(const ir::instr::Instr *instr,
                  const ir::MemPlaceInfo *memPlaceInfo, EventType eventType)
      : instr(instr), memPlaceInfo(memPlaceInfo), eventType(eventType) {}
  const ir::instr::Instr *instr;
  const ir::MemPlaceInfo *memPlaceInfo;
  const EventType eventType = EventType::Die;
};

using FreeRegisterPool =
    std::vector<std::pair<uint16_t, std::vector<const ir::MemPlaceInfo *>>>;
using FreeStackSlotPool =
    std::list<std::pair<uint16_t, std::vector<const ir::MemPlaceInfo *>>>;
using AliveInstrEvents =
    std::unordered_map<ir::instr::Instr::live_t, std::list<AliveInstrEvent>>;

template <typename MemPoolConstainer>
static const ir::MemPlaceInfo *TryGetMemPlace(MemPoolConstainer &pool,
                                              uint16_t size) {
  for (auto &[poolSize, sizedPool] : pool) {
    if (poolSize < size || sizedPool.empty()) {
      continue;
    }

    auto foundPlace = sizedPool.back();
    sizedPool.pop_back();
    return foundPlace;
  }

  return nullptr;
}

template <typename MemPoolConstainer>
static void ReturnMemPlace(MemPoolConstainer &pool,
                           const ir::MemPlaceInfo *memPlaceInfo) {
  auto it =
      std::find_if(pool.begin(), pool.end(), [&memPlaceInfo](const auto &pair) {
        return pair.first == memPlaceInfo->GetMaxSize();
      });
  ASSERT(it != pool.end())
  it->second.push_back(memPlaceInfo);
}

static size_t GensymId = 0;
static size_t GenSymId() { return GensymId++; }

static const ir::MemPlaceInfo *
AllocateStackSlot(std::vector<ir::MemPlaceInfo *> &stack, uint16_t size,
                  FreeStackSlotPool &freeStackSlots) {
  auto id = GenSymId();
  auto slot = stack.emplace_back(
      new ir::MemPlaceInfo("st" + std::to_string(id), false, false, size, id));

  {
    auto it = std::find_if(
        freeStackSlots.begin(), freeStackSlots.end(),
        [&slot](const auto &pair) { return pair.first == slot->GetMaxSize(); });

    if (it != freeStackSlots.end()) {
      return slot;
    }
  }

  if (freeStackSlots.empty()) {
    freeStackSlots.emplace_back(slot->GetMaxSize(),
                                std::vector<const ir::MemPlaceInfo *>());
    return slot;
  }

  auto it = freeStackSlots.begin();
  auto prev = it;

  while (it != freeStackSlots.end() && it->first < slot->GetMaxSize()) {
    prev = it;
    ++it;
  }

  if (it == prev) {
    freeStackSlots.emplace_front(slot->GetMaxSize(),
                                 std::vector<const ir::MemPlaceInfo *>());
    return slot;
  }

  freeStackSlots.emplace(prev, slot->GetMaxSize(),
                         std::vector<const ir::MemPlaceInfo *>());
  return slot;
}

static void InitFreePool(ir::MethodGraph &methodGraph,
                         FreeRegisterPool &freeIntRegisters,
                         FreeRegisterPool &freeFpRegisters) {
  for (auto &memPlaceInfo : methodGraph.GetRegistersInfo()) {
    auto &pool =
        memPlaceInfo.IsFloatingPoint() ? freeFpRegisters : freeIntRegisters;
    auto it = std::find_if(pool.begin(), pool.end(),
                           [&memPlaceInfo](const auto &pair) {
                             return pair.first == memPlaceInfo.GetMaxSize();
                           });

    if (it == pool.end()) {
      auto &emplacedPool = pool.emplace_back(
          memPlaceInfo.GetMaxSize(), std::vector<const ir::MemPlaceInfo *>());
      emplacedPool.second.push_back(&memPlaceInfo);
      continue;
    }
    it->second.push_back(&memPlaceInfo);
  }

  std::sort(freeIntRegisters.begin(), freeIntRegisters.end(),
            [](const auto &a, const auto &b) { return a.first < b.first; });
  std::sort(freeFpRegisters.begin(), freeFpRegisters.end(),
            [](const auto &a, const auto &b) { return a.first < b.first; });
}

static void FreeMemPlace(const ir::MemPlaceInfo *memPlaceInfo,
                         FreeStackSlotPool &freeStackSlots,
                         FreeRegisterPool &freeIntRegisters,
                         FreeRegisterPool &freeFpRegisters) {
  if (!memPlaceInfo->IsReg()) {
    ReturnMemPlace<FreeStackSlotPool>(freeStackSlots, memPlaceInfo);
    return;
  }

  ReturnMemPlace<FreeRegisterPool>(
      memPlaceInfo->IsFloatingPoint() ? freeFpRegisters : freeIntRegisters,
      memPlaceInfo);
}

// Area of improvement: deal with 'uses' and ranges with holes in them (support
// other event types (Use, StartInterval, EndInterval))
static void ProcessCurrentEvents(ir::instr::Instr::live_t currentLive,
                                 FreeStackSlotPool &freeStackSlots,
                                 FreeRegisterPool &freeIntRegisters,
                                 FreeRegisterPool &freeFpRegisters,
                                 AliveInstrEvents &aliveInstrEvents) {
  auto events = aliveInstrEvents.find(currentLive);
  if (events == aliveInstrEvents.end()) {
    return;
  }

  for (auto event : events->second) {
    if (event.eventType == EventType::Die) {
      auto memPlaceInfo = event.memPlaceInfo;
      FreeMemPlace(memPlaceInfo, freeStackSlots, freeIntRegisters,
                   freeFpRegisters);
    }
  }

  aliveInstrEvents.erase(currentLive);
}

// Area of improvement: analyze 'uses' while allocating registers, deal with
// ranges with holes in them
static void IterateThroughInstructions(
    ir::MethodGraph &methodGraph, std::vector<ir::MemPlaceInfo *> &stackSlots,
    FreeStackSlotPool &freeStackSlots, FreeRegisterPool &freeIntRegisters,
    FreeRegisterPool &freeFpRegisters, AliveInstrEvents &aliveInstrEvents) {
  for (auto block : methodGraph.GetLinearOrderedBlocks()) {
    for (auto instr = block->GetFirstInstr(); instr != nullptr;
         instr = instr->GetNextInstr()) {
      ProcessCurrentEvents(instr->GetLiveRange().GetLive(), freeStackSlots,
                           freeIntRegisters, freeFpRegisters, aliveInstrEvents);

      if (instr->GetLiveRange().GetRange().first ==
          instr->GetLiveRange().GetRange().second) {
        continue;
      }

      auto instrValSize = ir::instr::TypeIdToSize(instr->GetType());
      auto isFp = ir::instr::TypeIdIsFloatingPoint(instr->GetType());

      auto tryGetFreeRegister = TryGetMemPlace<FreeRegisterPool>(
          isFp ? freeFpRegisters : freeIntRegisters, instrValSize);
      if (tryGetFreeRegister != nullptr) {
        instr->GetRegAllocInfo().AddAllocationRange(
            instr->GetLiveRange().GetRange(), tryGetFreeRegister);
        aliveInstrEvents[instr->GetLiveRange().GetRange().second].emplace_back(
            instr, tryGetFreeRegister, EventType::Die);
        continue;
      }

      auto tryGetFreeStackSlot =
          TryGetMemPlace<FreeStackSlotPool>(freeStackSlots, instrValSize);
      if (tryGetFreeStackSlot != nullptr) {
        instr->GetRegAllocInfo().AddAllocationRange(
            instr->GetLiveRange().GetRange(), tryGetFreeStackSlot);
        aliveInstrEvents[instr->GetLiveRange().GetRange().second].emplace_back(
            instr, tryGetFreeStackSlot, EventType::Die);
        continue;
      }

      auto newStackSlot =
          AllocateStackSlot(stackSlots, instrValSize, freeStackSlots);
      instr->GetRegAllocInfo().AddAllocationRange(
          instr->GetLiveRange().GetRange(), newStackSlot);
      aliveInstrEvents[instr->GetLiveRange().GetRange().second].emplace_back(
          instr, newStackSlot, EventType::Die);
    }
  }
}

void RegAlloc::Allocate() {
  std::vector<ir::MemPlaceInfo *> stackSlots;

  FreeStackSlotPool freeStackSlots;
  FreeRegisterPool freeIntRegisters;
  FreeRegisterPool freeFpRegisters;
  AliveInstrEvents aliveInstrEvents;

  InitFreePool(graph_, freeIntRegisters, freeFpRegisters);

  if (graph_.GetLinearOrderedBlocks().empty()) {
    UNREACHABLE();
    return;
  }

  IterateThroughInstructions(graph_, stackSlots, freeStackSlots,
                             freeIntRegisters, freeFpRegisters,
                             aliveInstrEvents);
  graph_.SetStackSlotsInfo(std::move(stackSlots));
}

} // namespace analyzer