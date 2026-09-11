#ifndef BLIB_RANDOM_POISSONFIELD_HPP
#define BLIB_RANDOM_POISSONFIELD_HPP

#include <glm/glm.hpp>
#include <random>
#include <vector>

namespace bl
{
namespace rand
{
/**
 * @brief Helper for generating a poisson disk distribution of points in 2D space
 *
 * @ingroup Random
 */
class PoissonField {
public:
    /**
     * @brief Creates an empty poisson field
     */
    PoissonField() = default;

    /**
     * @brief Copy constructor
     */
    PoissonField(const PoissonField&) = default;

    /**
     * @brief Move constructor
     */
    PoissonField(PoissonField&&) = default;

    /**
     * @brief Creates a poisson field with the given parameters
     *
     * @param size The size of the field in world coordinates
     * @param minDistance The minimum distance between points
     */
    PoissonField(const glm::vec2& size, float minDistance);

    /**
     * @brief Copy assignment operator
     */
    PoissonField& operator=(const PoissonField&) = default;

    /**
     * @brief Move assignment operator
     */
    PoissonField& operator=(PoissonField&&) = default;

    /**
     * @brief Generates a new poisson field with the given parameters
     *
     * @param size The size of the field in world coordinates
     * @param minDistance The minimum distance between points
     */
    void generate(const glm::vec2& size, float minDistance);

    /**
     * @brief Returns the generated sample points
     */
    const std::vector<glm::vec2>& samples() const { return samplePoints; }

private:
    std::vector<glm::vec2> samplePoints;
};

} // namespace rand
} // namespace bl

#endif
