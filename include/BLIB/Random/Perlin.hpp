#ifndef BLIB_RANDOM_PERLIN_HPP
#define BLIB_RANDOM_PERLIN_HPP

#include <array>
#include <cmath>
#include <cstdint>
#include <numeric>
#include <random>

namespace bl
{
/// Collection of utilities for randomness and noise
namespace rand
{
/**
 * @brief Perlin noise generator
 *
 * @tparam T The type of random value to generate
 * @ingroup Random
 */
template<typename T>
class Perlin {
public:
    /**
     * @brief Seeds the generator with the default permutation
     */
    Perlin();

    /**
     * @brief Seeds the generator with an explicit seed
     *
     * @param seed The seed to shuffle with. Deterministic
     */
    Perlin(std::uint64_t seed);

    /**
     * @brief Seeds the generator with an explicit seed
     *
     * @param seed The seed to shuffle with. Deterministic
     */
    void reseed(std::uint64_t seed);

    /**
     * @brief Generates a random value in the range [-1,1] for the given 1d coordinate
     *
     * @param x The x coordinate to sample at
     * @return The random value in the range [-1,1]
     */
    T noise1D(T x) const;

    /**
     * @brief Generates a random value in the range [-1,1] for the given 2d coordinate
     *
     * @param x The x coordinate to sample at
     * @param y The y coordinate to sample at
     * @return T The random value in the range [-1,1]
     */
    T noise2D(T x, T y) const;

    /**
     * @brief Generates a random value in the range [-1,1] for the given 3d coordinate
     *
     * @param x The x coordinate to sample at
     * @param y The y coordinate to sample at
     * @param z The z coordinate to sample at
     * @return T The random value in the range [-1,1]
     */
    T noise3D(T x, T y, T z) const;

    /**
     * @brief Generates an octave noise value for the given 1d coordinate. Range is determined by
     *        octaves and persistence. See normalized version and maxAmplitude for more information
     *
     * @param x The x coordinate to sample at
     * @param octaves The number of octaves to sample
     * @param persistence The persistence value to use for amplitude scaling
     * @return The octave noise value
     */
    T octave1D(T x, unsigned int octaves, T persistence) const;

    /**
     * @brief Generates an octave noise value for the given 2d coordinate. Range is determined
     *        by octaves and persistence. See normalized version and maxAmplitude for more
     * information
     *
     * @param x The x coordinate to sample at
     * @param y The y coordinate to sample at
     * @param octaves The number of octaves to sample
     * @param persistence The persistence value to use for amplitude scaling
     * @return The octave noise value
     */
    T octave2D(T x, T y, unsigned int octaves, T persistence) const;

    /**
     * @brief Generates an octave noise value for the given 3d coordinate. Range is determined by
     *        octaves and persistence. See normalized version and maxAmplitude for more information
     *
     * @param x The x coordinate to sample at
     * @param y The y coordinate to sample at
     * @param z The z coordinate to sample at
     * @param octaves The number of octaves to sample
     * @param persistence The persistence value to use for amplitude scaling
     * @return The octave noise value
     */
    T octave3D(T x, T y, T z, unsigned int octaves, T persistence) const;

    /**
     * @brief Returns the maximum possible amplitude for the given number of octaves and persistence
     *
     * @param octaves The number of octave passes
     * @param persistence The amplitude scaling factor for each octave
     * @return The maximum possible amplitude for the given number of octaves and persistence
     */
    T maxAmplitude(unsigned int octaves, T persistence) const;

    /**
     * @brief Returns the normalized octave noise value for the given 1d coordinate. Range is [-1,1]
     *
     * @param x The x coordinate to sample at
     * @param octaves The number of octaves to sample
     * @param persistence The persistence value to use for amplitude scaling
     * @return The normalized octave noise value in the range [-1,1]
     */
    T octave1DNormalized(T x, unsigned int octaves, T persistence) const;

    /**
     * @brief Returns the normalized octave noise value for the given 2d coordinate. Range is [-1,1]
     *
     * @param x The x coordinate to sample at
     * @param y The y coordinate to sample at
     * @param octaves The number of octaves to sample
     * @param persistence The persistence value to use for amplitude scaling
     * @return The normalized octave noise value in the range [-1,1]
     */
    T octave2DNormalized(T x, T y, unsigned int octaves, T persistence) const;

    /**
     * @brief Returns the normalized octave noise value for the given 3d coordinate. Range is [-1,1]
     *
     * @param x The x coordinate to sample at
     * @param y The y coordinate to sample at
     * @param z The z coordinate to sample at
     * @param octaves The number of octaves to sample
     * @param persistence The persistence value to use for amplitude scaling
     * @return The normalized octave noise value in the range [-1,1]
     */
    T octave3DNormalized(T x, T y, T z, unsigned int octaves, T persistence) const;

private:
    std::array<std::uint8_t, 256> permutation;

    static constexpr T DefaultY = static_cast<T>(0.12345);
    static constexpr T DefaultZ = static_cast<T>(0.34567);

    T fade(T t) const;
    T lerp(T a, T b, T t) const;
    T grad(std::uint8_t hash, T x, T y, T z) const;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

template<typename T>
Perlin<T>::Perlin()
: permutation{
      151, 160, 137, 91,  90,  15,  131, 13,  201, 95,  96,  53,  194, 233, 7,   225, 140, 36,  103,
      30,  69,  142, 8,   99,  37,  240, 21,  10,  23,  190, 6,   148, 247, 120, 234, 75,  0,   26,
      197, 62,  94,  252, 219, 203, 117, 35,  11,  32,  57,  177, 33,  88,  237, 149, 56,  87,  174,
      20,  125, 136, 171, 168, 68,  175, 74,  165, 71,  134, 139, 48,  27,  166, 77,  146, 158, 231,
      83,  111, 229, 122, 60,  211, 133, 230, 220, 105, 92,  41,  55,  46,  245, 40,  244, 102, 143,
      54,  65,  25,  63,  161, 1,   216, 80,  73,  209, 76,  132, 187, 208, 89,  18,  169, 200, 196,
      135, 130, 116, 188, 159, 86,  164, 100, 109, 198, 173, 186, 3,   64,  52,  217, 226, 250, 124,
      123, 5,   202, 38,  147, 118, 126, 255, 82,  85,  212, 207, 206, 59,  227, 47,  16,  58,  17,
      182, 189, 28,  42,  223, 183, 170, 213, 119, 248, 152, 2,   44,  154, 163, 70,  221, 153, 101,
      155, 167, 43,  172, 9,   129, 22,  39,  253, 19,  98,  108, 110, 79,  113, 224, 232, 178, 185,
      112, 104, 218, 246, 97,  228, 251, 34,  242, 193, 238, 210, 144, 12,  191, 179, 162, 241, 81,
      51,  145, 235, 249, 14,  239, 107, 49,  192, 214, 31,  181, 199, 106, 157, 184, 84,  204, 176,
      115, 121, 50,  45,  127, 4,   150, 254, 138, 236, 205, 93,  222, 114, 67,  29,  24,  72,  243,
      141, 128, 195, 78,  66,  215, 61,  156, 180} {}

template<typename T>
Perlin<T>::Perlin(std::uint64_t seed)
: Perlin() {
    reseed(seed);
}

template<typename T>
void Perlin<T>::reseed(std::uint64_t seed) {
    std::mt19937 generator(seed);
    std::iota(permutation.begin(), permutation.end(), 0);

    for (unsigned int i = 1; i < permutation.size(); ++i) {
        const unsigned int j = generator() % (i + 1);
        std::swap(permutation[i], permutation[j]);
    }
}

template<typename T>
T Perlin<T>::noise1D(T x) const {
    return noise3D(x, DefaultY, DefaultZ);
}

template<typename T>
T Perlin<T>::noise2D(T x, T y) const {
    return noise3D(x, y, DefaultZ);
}

template<typename T>
T Perlin<T>::noise3D(T x, T y, T z) const {
    const T _x = std::floor(x);
    const T _y = std::floor(y);
    const T _z = std::floor(z);

    const std::int32_t ix = static_cast<std::int32_t>(_x) & 255;
    const std::int32_t iy = static_cast<std::int32_t>(_y) & 255;
    const std::int32_t iz = static_cast<std::int32_t>(_z) & 255;

    const T fx = (x - _x);
    const T fy = (y - _y);
    const T fz = (z - _z);

    const T u = fade(fx);
    const T v = fade(fy);
    const T w = fade(fz);

    const std::uint8_t A = (permutation[ix & 255] + iy) & 255;
    const std::uint8_t B = (permutation[(ix + 1) & 255] + iy) & 255;

    const std::uint8_t AA = (permutation[A] + iz) & 255;
    const std::uint8_t AB = (permutation[(A + 1) & 255] + iz) & 255;

    const std::uint8_t BA = (permutation[B] + iz) & 255;
    const std::uint8_t BB = (permutation[(B + 1) & 255] + iz) & 255;

    const T p0 = grad(permutation[AA], fx, fy, fz);
    const T p1 = grad(permutation[BA], fx - 1, fy, fz);
    const T p2 = grad(permutation[AB], fx, fy - 1, fz);
    const T p3 = grad(permutation[BB], fx - 1, fy - 1, fz);
    const T p4 = grad(permutation[(AA + 1) & 255], fx, fy, fz - 1);
    const T p5 = grad(permutation[(BA + 1) & 255], fx - 1, fy, fz - 1);
    const T p6 = grad(permutation[(AB + 1) & 255], fx, fy - 1, fz - 1);
    const T p7 = grad(permutation[(BB + 1) & 255], fx - 1, fy - 1, fz - 1);

    const T q0 = lerp(p0, p1, u);
    const T q1 = lerp(p2, p3, u);
    const T q2 = lerp(p4, p5, u);
    const T q3 = lerp(p6, p7, u);

    const T r0 = lerp(q0, q1, v);
    const T r1 = lerp(q2, q3, v);

    return lerp(r0, r1, w);
}

template<typename T>
T Perlin<T>::octave1D(T x, unsigned int octaves, T persistence) const {
    T result    = 0;
    T amplitude = 1;

    for (unsigned int i = 0; i < octaves; ++i) {
        result += noise1D(x) * amplitude;
        x *= 2;
        amplitude *= persistence;
    }

    return result;
}

template<typename T>
T Perlin<T>::octave2D(T x, T y, unsigned int octaves, T persistence) const {
    T result    = 0;
    T amplitude = 1;

    for (unsigned int i = 0; i < octaves; ++i) {
        result += noise2D(x, y) * amplitude;
        x *= 2;
        y *= 2;
        amplitude *= persistence;
    }

    return result;
}

template<typename T>
T Perlin<T>::octave3D(T x, T y, T z, unsigned int octaves, T persistence) const {
    T result    = 0;
    T amplitude = 1;

    for (unsigned int i = 0; i < octaves; ++i) {
        result += noise3D(x, y, z) * amplitude;
        x *= 2;
        y *= 2;
        z *= 2;
        amplitude *= persistence;
    }

    return result;
}

template<typename T>
inline T Perlin<T>::maxAmplitude(unsigned int octaves, T persistence) const {
    T result    = 0;
    T amplitude = 1;

    for (unsigned int i = 0; i < octaves; ++i) {
        result += amplitude;
        amplitude *= persistence;
    }

    return result;
}

template<typename T>
inline T Perlin<T>::octave1DNormalized(T x, unsigned int octaves, T persistence) const {
    return octave1D(x, octaves, persistence) / maxAmplitude(octaves, persistence);
}

template<typename T>
inline T Perlin<T>::octave2DNormalized(T x, T y, unsigned int octaves, T persistence) const {
    return octave2D(x, y, octaves, persistence) / maxAmplitude(octaves, persistence);
}

template<typename T>
inline T Perlin<T>::octave3DNormalized(T x, T y, T z, unsigned int octaves, T persistence) const {
    return octave3D(x, y, z, octaves, persistence) / maxAmplitude(octaves, persistence);
}

template<typename T>
T Perlin<T>::fade(T t) const {
    return t * t * t * (t * (t * 6 - 15) + 10);
}

template<typename T>
T Perlin<T>::lerp(T a, T b, T t) const {
    return a + (b - a) * t;
}

template<typename T>
T Perlin<T>::grad(std::uint8_t hash, T x, T y, T z) const {
    const std::uint8_t h = hash & 15;
    const T u            = h < 8 ? x : y;
    const T v            = h < 4 ? y : h == 12 || h == 14 ? x : z;
    return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
}

} // namespace rand
} // namespace bl

#endif
