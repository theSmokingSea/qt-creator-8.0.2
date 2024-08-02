#pragma once

#include <qsystemdetection.h>

#include <cstdlib>
#include <cstring>
#include <memory>

namespace Utils {

namespace Memory {

inline char *allocate(std::size_t size)
{
    return static_cast<char*>(std::malloc(size));
}

inline void deallocate(char *memory)
{
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wfree-nonheap-object"
#endif
    std::free(memory);
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic pop
#endif
}

inline char *reallocate(char *oldMemory, std::size_t newSize)
{
    return static_cast<char *>(std::realloc(oldMemory, newSize));
}

} // namespace Memory

} // namespace Utils
