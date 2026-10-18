#ifndef BLIB_RANDOM_PERLINSAMPLER_HPP
#define BLIB_RANDOM_PERLINSAMPLER_HPP

#include <BLIB/Random/Perlin.hpp>
#include <BLIB/Random/PerlinParameters.hpp>
#include <glm/glm.hpp>

namespace bl
{
namespace rand
{
class Seed;

/**
 * @brief Helper class to encode perlin parameters for easy sampling
 *
 * @ingroup Random
 */
class PerlinSampler {
public:
    /**
     * @brief Creates an empty sampler
     */
    PerlinSampler() = default;

    /**
     * @brief Creates a sampler with the given parameters and origin
     *
     * @param params The perlin noise parameters
     * @param origin The origin in world coordinates
     * @param seed The seed to use
     * @param id The perlin noise id to use
     */
    PerlinSampler(const PerlinParameters& params, const glm::vec1& origin,
                  const bl::rand::Seed& seed, std::uint64_t id = 0);

    /**
     * @brief Creates a sampler with the given parameters and origin
     *
     * @param params The perlin noise parameters
     * @param origin The origin in world coordinates
     * @param seed The seed to use
     * @param id The perlin noise id to use
     */
    PerlinSampler(const PerlinParameters& params, const glm::vec2& origin,
                  const bl::rand::Seed& seed, std::uint64_t id = 0);

    /**
     * @brief Creates a sampler with the given parameters and origin
     *
     * @param params The perlin noise parameters
     * @param origin The origin in world coordinates
     * @param seed The seed to use
     * @param id The perlin noise id to use
     */
    PerlinSampler(const PerlinParameters& params, const glm::vec3& origin,
                  const bl::rand::Seed& seed, std::uint64_t id = 0);

    /**
     * @brief Copies the sampler
     *
     * @param copy The sampler to copy
     */
    PerlinSampler(const PerlinSampler& copy) = default;

    /**
     * @brief Samples the perlin noise at the given position
     *
     * @param pos The position in world coordinates to sample at
     * @return The perlin noise value in range [0, 1]
     */
    float sample(float pos) const;

    /**
     * @brief Samples the perlin noise at the given position
     *
     * @param pos The position in world coordinates to sample at
     * @return The perlin noise value in range [0, 1]
     */
    float sample(const glm::vec2& pos) const;

    /**
     * @brief Samples the perlin noise at the given position
     *
     * @param pos The position in world coordinates to sample at
     * @return The perlin noise value in range [0, 1]
     */
    float sample(const glm::vec3& pos) const;

private:
    bl::rand::Perlin<float> perlin;
    glm::vec3 origin;
    PerlinParameters params;
};

} // namespace rand
} // namespace bl

#endif
