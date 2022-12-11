#ifndef BLIB_UTIL_RANDOM_HPP
#define BLIB_UTIL_RANDOM_HPP

#include <BLIB/Util/NonCopyable.hpp>

#include <algorithm>
#include <chrono>
#include <limits>
#include <random>
#include <type_traits>

/// The parent namespace for all of BLIB
namespace bl
{
/// Contains a set of common utility classes that do not fit into any other modules
namespace util
{
/**
 * @brief Random number generator for integral and floating point numbers
 *
 * @ingroup Util
 */
class Random : private util::NonCopyable {
    template<typename U>
    struct CharHelper {
        using T = U;
    };

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
        using U = typename CharHelper<T>::T;
        std::uniform_int_distribution<U> rgen(static_cast<U>(min), static_cast<U>(max));
        return rgen(_priv().rng);
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
        using U = std::underlying_type_t<T>;
        return static_cast<T>(get<U>(static_cast<U>(min), static_cast<U>(max)));
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
        std::uniform_real_distribution<T> rgen(min, max);
        return rgen(_priv().rng);
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
        std::shuffle(begin, end, Shuffler());
    }

private:
    Random()
    : rng(rngdev()) {}

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

    std::random_device rngdev;
    std::mt19937 rng;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

template<>
struct Random::CharHelper<char> {
    using T = short;
};

template<>
struct Random::CharHelper<unsigned char> {
    using T = unsigned short;
};

} // namespace util
} // namespace bl

#endif
