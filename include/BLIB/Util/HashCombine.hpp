#ifndef BLIB_UTIL_HASHCOMBINE_HPP
#define BLIB_UTIL_HASHCOMBINE_HPP

#include <cstddef>
#include <cstdint>

namespace bl
{
namespace util
{
/**
 * @brief Combines two hash values
 *
 * @param left The left value to combine
 * @param right The right value to combine
 * @return The combined hash value
 */
inline std::size_t hashCombine(std::size_t left, std::size_t right) {
    right ^= left + 0x9e3779b9 + (right << 6) + (right >> 2);
    return right;
}

} // namespace util
} // namespace bl

#endif
