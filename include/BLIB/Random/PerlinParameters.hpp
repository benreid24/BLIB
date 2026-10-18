#ifndef BLIB_RANDOM_PERLINPARAMETERS_HPP
#define BLIB_RANDOM_PERLINPARAMETERS_HPP

namespace bl
{
namespace rand
{
/**
 * @brief Parameters specific to the Perlin noise generator
 *
 * @ingroup Random
 */
struct PerlinParameters {
    float frequency;
    unsigned int octaves;
    float persistence;

    /**
     * @brief Sets sane defaults
     */
    PerlinParameters()
    : frequency(0.01f)
    , octaves(16)
    , persistence(0.5f) {}
};

} // namespace rand
} // namespace bl

#endif
