#include "livenessAnalyzer.h"
#include <unordered_map>
#include <set>

namespace analyzer {

namespace {

size_t GetPredecessorIndex(ir::BasicBlock* pred, ir::BasicBlock* succ) {
    const auto& preds = succ->GetPreds();
    for (size_t i = 0; i < preds.size(); ++i) {
        if (preds[i] == pred) {
            return i;
        }
    }
    return -1;
}

void ShortenRangeStart(ir::instr::Instr* instr, ir::instr::Instr::live_t blockFrom) {
    auto& lr = instr->GetLiveRange();
    auto livePos = lr.GetLive();

    if (lr.GetIntervals().empty()) {
        lr.AddInterval({livePos, livePos});
        return;
    }

    auto intervals = lr.GetIntervals();
    bool modified = false;

    for (auto& interval : intervals) {
        if (interval.first == blockFrom) {
            interval.first = livePos;
            modified = true;
        }
    }

    if (modified) {
        lr.SetIntervals(intervals);
        auto min = intervals.empty() ? 0 : intervals[0].first;
        auto max = intervals.empty() ? 0 : intervals[0].second;
        for(const auto& iv : intervals) {
            if (iv.first < min) min = iv.first;
            if (iv.second > max) max = iv.second;
        }
        lr.SetRange({min, max});
    }
}

size_t GetLoopEndPos(analyzer::Loop* loop) {
    if (!loop) return 0;
    size_t maxPos = 0;
    for (auto* node : loop->GetNodes()) {
        if (node && node->first) {
            size_t blockEnd = node->first->GetLiveRange().second;
            if (blockEnd > maxPos) {
                maxPos = blockEnd;
            }
        }
    }
    return maxPos;
}

}

void LivenessAnalyzer::InitLiveLinForInstr()
{
    size_t countLive = 0;
    size_t countLin = 0;
    for (auto &block : graph_.GetLinearOrderedBlocks()) {
        auto minLive = countLive;
        auto currentInstr = block->GetFirstInstr();
        while (currentInstr != nullptr) {
            if (currentInstr->GetOpcode() != ir::instr::InstrOpcode::PHI) {
                countLive += 2;
            }
            currentInstr->GetLiveRange().SetLive(countLive);
            currentInstr->GetLiveRange().SetLin(countLin);
            currentInstr->GetLiveRange().SetIsRangeSet(false);
            countLin++;
            currentInstr = currentInstr->GetNextInstr();
        }
        countLive += 2;
        block->SetLiveRange({minLive, countLive});
    }
}

void LivenessAnalyzer::CalculateLiveRanges()
{
    InitLiveLinForInstr();

    std::unordered_map<ir::BasicBlock*, std::set<ir::instr::Instr*>> liveInMap;
    const auto& blocks = graph_.GetLinearOrderedBlocks();

    for (auto it = blocks.rbegin(); it != blocks.rend(); ++it) {
        ir::BasicBlock* block = *it;
        auto blockFrom = block->GetLiveRange().first;

        std::set<ir::instr::Instr*> live;

        for (auto* succ : block->GetSuccs()) {
            if (succ == nullptr) continue;
            if (liveInMap.count(succ)) {
                const auto& succLiveIn = liveInMap.at(succ);
                live.insert(succLiveIn.begin(), succLiveIn.end());
            }
        }

        for (auto* succ : block->GetSuccs()) {
            if (succ == nullptr) continue;
            size_t predIndex = GetPredecessorIndex(block, succ);
            if (predIndex == static_cast<size_t>(-1)) continue;

            for (auto* phi = succ->GetFirstInstr(); phi != nullptr && phi->IsPhiInstr(); phi = phi->GetNextInstr()) {
                const auto& inputs = phi->GetInputs();
                if (predIndex < inputs.size()) {
                    auto* inputOp = inputs[predIndex];
                    if (inputOp != nullptr) {
                        live.insert(inputOp);
                    }
                }
            }
        }

        for (auto* opd : live) {
            opd->GetLiveRange().AddInterval({block->GetLiveRange().first, block->GetLiveRange().second});
        }

        for (auto* currentInstr = block->GetLastInstr(); currentInstr != nullptr; currentInstr = currentInstr->GetPrevInstr()) {
            if (currentInstr->IsPhiInstr()) continue;

            ShortenRangeStart(currentInstr, blockFrom);
            live.erase(currentInstr);

            for (auto* input : currentInstr->GetInputs()) {
                if (input != nullptr) {
                    input->GetLiveRange().AddInterval({blockFrom, currentInstr->GetLiveRange().GetLive()});
                    live.insert(input);
                }
            }
        }

        for (auto* phi = block->GetFirstInstr(); phi != nullptr && phi->IsPhiInstr(); phi = phi->GetNextInstr()) {
            live.erase(phi);
        }

        if (block->GetLoop() != nullptr) {
            auto loop = block->GetLoop();
            if (loop->GetHead() && loop->GetHead()->first == block) {
                size_t loopEndPos = GetLoopEndPos(loop);
                for (auto* opd : live) {
                    opd->GetLiveRange().AddInterval({blockFrom, loopEndPos});
                }
            }
        }

        liveInMap[block] = std::move(live);
    }
}

} // namespace analyzer