#ifndef BLIB_COMPONENTS_VELOCITY2D_HPP
#define BLIB_COMPONENTS_VELOCITY2D_HPP

#include <glm/glm.hpp>

namespace bl
{
namespace com
{
/**
 * @brief Represents a translational and rotational velocity in 2d space
 *
 * @ingroup Components
 */
struct Velocity2D {
    /**
     * @brief Initializes the velocity to 0
     */
    Velocity2D()
    : velocity(0.f, 0.f)
    , angularVelocity(0.f) {}

    /**
     * @brief Initializes with the given velocity and zero angular velocity
     *
     * @param velocity The speed to translate at in units per second
     */
    Velocity2D(const glm::vec2& velocity)
    : velocity(velocity)
    , angularVelocity(0.f) {}

    /**
     * @brief Initializes with the given velocity and angular velocity
     *
     * @param velocity The velocity to translate at in units per second
     * @param anglularVelocity The speed to rotate at in degrees per second
     */
    Velocity2D(const glm::vec2& velocity, float anglularVelocity)
    : velocity(velocity)
    , angularVelocity(angularVelocity) {}

    /// Velocity to apply to the position in units per second
    glm::vec2 velocity;

    // Angular speed in degrees per second
    float angularVelocity;
};

} // namespace com
} // namespace bl

#endif
