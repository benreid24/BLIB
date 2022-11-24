#ifndef BLIB_UTIL_HASHCOMBINE_HPP
#define BLIB_UTIL_HASHCOMBINE_HPP

#include <cstddef>
#include <cstdint>

namespace bl
{
namespace util
{
inline std::size_t hashCombine(std::size_t right, std::size_t left) {
    right ^= left + 0x9e3779b9 + (right << 6) + (right >> 2);
    return right;
}

} // namespace util
} // namespace bl

#endif
