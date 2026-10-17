#ifndef BLIB_RANDOM_SEED_HPP
#define BLIB_RANDOM_SEED_HPP

#include <BLIB/Random/Perlin.hpp>
#include <algorithm>
#include <cstdint>
#include <random>
#include <type_traits>

namespace bl
{
namespace rand
{
/**
 * @brief Random number seed and generator
 *
 * @ingroup Random
 */
class Seed {
    template<typename U>
    struct ResolveChar {
        using T = U;
    };

public:
    /**
     * @brief Creates the generator with a random seed
     */
    Seed();

    /**
     * @brief Creates the generator with the given seed
     *
     * @param seed The seed to use
     */
    Seed(std::uint64_t seed);

    /**
     * @brief Sets the seed to use
     *
     * @param seed The seed to use
     */
    void reseed(std::uint64_t seed);

    /**
     * @brief Gets the current seed
     */
    std::uint64_t getSeed() const { return seed; }

    /**
     * @brief Returns a Perlin noise generator seeded with the current seed
     *
     * @tparam T The type of random value to generate
     * @param offset Seed offset for the Perlin noise generator
     */
    template<typename T = float>
    Perlin<T> getPerlin(std::uint64_t offset = 0) const {
        return Perlin<T>(seed + offset);
    }

    /**
     * @brief Returns a random number in the given range of a float type
     *
     * @tparam T The float type to return
     * @param min Minimum value of the range
     * @param max Maximum value of the range
     * @return A random number in the range [min,max]
     */
    template<typename T = float>
    T getFloat(T min, T max) {
        static_assert(std::is_floating_point_v<T>, "getFloat requires a floating point type");
        if (min > max) std::swap(min, max);
        std::uniform_real_distribution<T> rgen(min, max);
        return rgen(rng);
    }

    /**
     * @brief Returns a random integer
     *
     * @param min The minimum value
     * @param max The maximum value
     * @return A random number in the range [min, max]
     */
    template<typename T = int>
    T getInt(T min, T max) {
        using U = typename ResolveChar<T>::T;
        static_assert(std::is_integral_v<U>, "getInt requires an integral type");
        if (min > max) std::swap(min, max);
        std::uniform_int_distribution<U> rgen(static_cast<U>(min), static_cast<U>(max));
        return static_cast<T>(rgen(rng));
    }

    /**
     * @brief Returns a random number in the given range of an enum type
     *
     * @tparam T The enum type to return
     * @param min Minimum value of the range
     * @param max Maximum value of the range
     * @return A random number in the range [min,max]
     */
    template<typename T>
    T getEnum(T min, T max) {
        static_assert(std::is_enum_v<T>, "getEnum requires an enum type");
        using U = std::underlying_type_t<T>;
        return static_cast<T>(getInt<U>(static_cast<U>(min), static_cast<U>(max)));
    }

    /**
     * @brief Returns true or false with probability equal to numerator/denominator
     *
     * @param numerator Odds of returning true
     * @param denominator Total odds
     * @return true or false with probability equal to numerator/denominator
     */
    bool chance(int numerator, int denominator) { return getInt(0, denominator) < numerator; }

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

    /**
     * @brief Returns the underlying random number generator
     */
    std::mt19937_64& getRNG() { return rng; }

private:
    std::uint64_t seed;
    std::mt19937_64 rng;

    struct Shuffler {
        using result_type = std::uint32_t;

        constexpr static result_type min() { return std::numeric_limits<result_type>::min(); }

        constexpr static result_type max() { return std::numeric_limits<result_type>::max(); }

        result_type operator()() const {
            std::uniform_int_distribution<result_type> dist(min(), max());
            return dist(rng);
        }

        Shuffler(std::mt19937_64& rng)
        : rng(rng) {}

        std::mt19937_64& rng;
    };
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

template<>
struct Seed::ResolveChar<char> {
    using T = int;
};

template<>
struct Seed::ResolveChar<unsigned char> {
    using T = unsigned int;
};

} // namespace rand
} // namespace bl

#endif
