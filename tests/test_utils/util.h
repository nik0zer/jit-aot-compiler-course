#ifndef UTIL_H
#define UTIL_H

#include <iostream>

namespace util {
enum class ReturnCode { SUCCESS = 0, FAILURE = 1 };

#define RETURN_IF_NOT_OK(expr)                                                 \
  do {                                                                         \
    auto _ret = (expr);                                                        \
    if (_ret != util::ReturnCode::SUCCESS)                                     \
      return static_cast<int>(_ret);                                           \
  } while (0)

inline ReturnCode CheckTrue(bool condition, const char *message) {
  if (!condition) {
    std::cerr << "Check failed: " << message << std::endl;
    return ReturnCode::FAILURE;
  }
  return ReturnCode::SUCCESS;
}

template <typename T>
inline ReturnCode CheckEqual(T a, T b, const char *message) {
  return CheckTrue(a == b, message);
}

} // namespace util

#endif