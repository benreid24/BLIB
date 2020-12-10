#ifndef BLIB_UTIL_PAIRHASH_HPP
#define BLIB_UTIL_PAIRHASH_HPP

#include <functional>

namespace bl
{
/**
 * @brief Utility struct to allow hashing of std::pair objects
 *
 * @tparam T1 Type of the first element
 * @tparam T2 Type of the second element
 * @tparam T1Hash Hasher to use for the first element
 * @tparam T2Hash Hasher to use for the second element
 *
 * @ingroup Util
 */
template<typename T1, typename T2, typename T1Hash = std::hash<T1>,
         typename T2Hash = std::hash<T2>>
struct PairHash {
    std::size_t operator()(const std::pair<T1, T2>& pair) const {
        std::size_t h1 = T1Hash()(pair.first);
        std::size_t h2 = T2Hash()(pair.second);
        h1 ^= h2 + 0x9e3779b9 + (h1 << 6) + (h1 >> 2);
        return h1;
    }
};

} // namespace bl

#endif