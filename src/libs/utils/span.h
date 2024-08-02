#pragma once

#if __cplusplus >= 202002L
#include <span>

namespace Utils {
using std::as_bytes;
using std::as_writable_bytes;
using std::get;
using std::span;
} // namespace Utils
#else
#define TCB_SPAN_NAMESPACE_NAME Utils
#include <3rdparty/span/span.hpp>
#endif
