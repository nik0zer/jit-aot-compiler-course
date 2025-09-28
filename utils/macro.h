#ifndef MACRO_H
#define MACRO_H

#include <cstdlib>
#include <iostream>

namespace util {

static void AssertionFail(const char *expr, const char *file, unsigned line,
                          const char *function) {
  std::cerr << "ASSERTION FAILED: " << expr << std::endl;
  std::cerr << "IN " << file << ":" << std::dec << line << ": " << function
            << std::endl;
  std::abort();
}

#define ASSERT_PRINT(cond, expr)                                               \
  if (!(cond))                                                                 \
    util::AssertionFail(expr, __FILE__, __LINE__, __func__);

#define ASSERT(cond) ASSERT_PRINT(cond, #cond)

#define UNREACHABLE() ASSERT_PRINT(false, "This line should be unreachable")

} // namespace util

#endif