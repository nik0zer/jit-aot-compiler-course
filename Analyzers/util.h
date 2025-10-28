#ifndef ANALYZERS_UTIL_H
#define ANALYZERS_UTIL_H

#include <cstddef>
#include <utility>

namespace ir {
class BasicBlock;
}

namespace Analyzer {

using BlockWithIndex = std::pair<ir::BasicBlock *, size_t>;

} // namespace Analyzer

#endif