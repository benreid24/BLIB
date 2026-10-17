#ifndef BLIB_RANDOM_RANDOM_HPP
#define BLIB_RANDOM_RANDOM_HPP

#include <BLIB/Random/Seed.hpp>
#include <BLIB/Util/NonCopyable.hpp>
#include <algorithm>
#include <chrono>
#include <limits>
#include <random>
#include <type_traits>

/// The parent namespace for all of BLIB
namespace bl
{
namespace rand
{
/**
 * @brief Random number generator for integral and floating point numbers
 *
 * @ingroup Random
 */
class Random : private util::NonCopyable {
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
        return _priv().seed.getInt(min, max);
    }

    /**
     * @brief Returns a random number in the given range of an enum type
     *
     * @tparam T The type of number to return
     * @param min Minimum value of the range
     * @param max Maximum value of the range
     * @return T A random number in the range [min,max]
     */
    template<typename T>
    static std::enable_if_t<std::is_enum_v<T>, T> get(T min, T max) {
        return _priv().seed.getEnum<T>(min, max);
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
        return _priv().seed.getFloat(min, max);
    }

    /**
     * @brief Returns true or false with probability equal to numerator/denominator
     *
     * @param numerator Odds of returning true
     * @param denominator Total odds
     * @return true or false with probability equal to numerator/denominator
     */
    static bool chance(int numerator, int denominator) {
        return get<int>(0, denominator) > numerator;
    }

    /**
     * @brief Helper function that utilizes the random number generation of this module to shuffle a
     *        sequence of values. Affected iterators are [begin, end)
     *
     * @tparam RandomIter Iterator type. Must be passable to std::shuffle
     * @param begin The begin iterator of the container to shuffle
     * @param end The end iterator of the container to shuffle
     */
    template<typename RandomIter>
    static void shuffle(RandomIter begin, RandomIter end) {
        _priv().seed.shuffle(begin, end);
    }

    /**
     * @brief Returns the underlying random number generator
     */
    static std::mt19937_64& getRNG() { return _priv().seed.getRNG(); }

    /**
     * @brief Returns the seed being used by the global random number generator
     */
    static Seed& getSeed() { return _priv().seed; }

private:
    Random() = default;

    static Random& _priv() {
        static Random rng;
        return rng;
    }

    struct Shuffler {
        using result_type = std::uint32_t;
        constexpr static result_type min() { return std::numeric_limits<result_type>::min(); }
        constexpr static result_type max() { return std::numeric_limits<result_type>::max(); }
        result_type operator()() const { return Random::get<result_type>(min(), max()); }
    };

    Seed seed;
};

} // namespace rand
} // namespace bl

#endif
