#ifndef BLIB_UTIL_HASHES_HPP
#define BLIB_UTIL_HASHES_HPP

#include <BLIB/Util/HashCombine.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/System/Vector3.hpp>
#include <functional>

namespace bl
{
namespace util
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
template<typename T1, typename T2, typename T1Hash = std::hash<T1>, typename T2Hash = std::hash<T2>>
struct PairHash {
    /**
     * @brief Hashes the given pair using the hash function for each member of the pair
     *
     * @param pair The pair to hash
     * @return std::size_t A hash of the items in the pair
     */
    std::size_t operator()(const std::pair<T1, T2>& pair) const {
        std::size_t h1 = T1Hash()(pair.first);
        std::size_t h2 = T2Hash()(pair.second);
        return hashCombine(h1, h2);
    }
};

/**
 * @brief Specialized hash for sf::Vector2
 *
 * @tparam T The type of vector to hash
 * @ingroup Util
 */
template<typename T>
struct Vector2Hash {
    /**
     * @brief Hash the given vector
     *
     * @param value The vector to hash
     * @return std::size_t The hash of the vector
     */
    std::size_t operator()(const sf::Vector2<T>& value) const {
        std::hash<T> hasher;
        return bl::util::hashCombine(hasher(value.x), hasher(value.y));
    }
};

/**
 * @brief Specialized hash for sf::Vector3
 *
 * @tparam T The type of vector to hash
 * @ingroup Util
 */
template<typename T>
struct Vector3Hash {
    /**
     * @brief Hash the given vector
     *
     * @param value The vector to hash
     * @return std::size_t The hash of the vector
     */
    std::size_t operator()(const sf::Vector3<T>& value) const {
        std::hash<T> hasher;
        return bl::util::hashCombine(bl::util::hashCombine(hasher(value.x), hasher(value.y)),
                                     hasher(value.z));
    }
};

} // namespace util
} // namespace bl

#endif
