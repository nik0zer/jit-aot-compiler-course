#ifndef ANALYZERS_UTIL_H
#define ANALYZERS_UTIL_H

#include "instructions/instr.h"
#include <cstddef>
#include <utility>

namespace ir {
class BasicBlock;
}

namespace analyzer {
class DominatorAnalyzer;

using BlockWithIndex = std::pair<ir::BasicBlock *, size_t>;

bool InstructionsDominates(ir::instr::Instr *dominator,
                           ir::instr::Instr *dominatee);

} // namespace analyzer

#endif