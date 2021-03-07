#ifndef BLIB_UTIL_RANDOM_HPP
#define BLIB_UTIL_RANDOM_HPP

#include <BLIB/Util/NonCopyable.hpp>

#include <chrono>
#include <limits>
#include <random>
#include <type_traits>

namespace bl
{
/**
 * @brief Random number generator for integral and floating point numbers
 *
 * @ingroup Util
 */
class Random : private NonCopyable {
public:
    /**
     * @brief Returns a random number in the given range of an integral type
     *
     * @tparam T The type of number to return
     * @param min Minimum value of the range
     * @param max Maximum value of the range
     * @return T A random number in the range [min,max]
     */
    template<typename T>
    static std::enable_if_t<std::is_integral_v<T>, T> get(T min, T max) {
        if (min > max) std::swap(min, max);
        std::uniform_int_distribution<std::mt19937::result_type> rgen(min, max);
        return rgen(_priv().rng);
    }

    /**
     * @brief Returns a random number in the given range of a float type
     *
     * @tparam T The type of number to return
     * @param min Minimum value of the range
     * @param max Maximum value of the range
     * @return T A random number in the range [min,max]
     */
    template<typename T>
    static std::enable_if_t<std::is_floating_point_v<T>, T> get(T min, T max) {
        if (min > max) std::swap(min, max);
        const uint32_t range = std::numeric_limits<uint32_t>::max();
        const T point        = get<uint32_t>(0, range);
        return min + point / static_cast<T>(range) * (max - min);
    }

private:
    Random()
    : rng(rngdev()) {}
    static Random& _priv() {
        static Random rng;
        rng.rng.seed(std::chrono::high_resolution_clock::now().time_since_epoch().count());
        return rng;
    }

    std::random_device rngdev;
    std::mt19937 rng;
};

} // namespace bl

#endif