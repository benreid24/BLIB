#ifndef BLIB_SYSTEMS_PHYSICS2D_HPP
#define BLIB_SYSTEMS_PHYSICS2D_HPP

#include <BLIB/Engine/System.hpp>
#include <box2d/box2d.h>
#include <glm/glm.hpp>

namespace bl
{
namespace sys
{
/**
 * @brief Physics system for 2d physics. Wraps Box2D and adapts the ECS to it
 *
 * @ingroup Systems
 */
class Physics2D : public engine::System {
public:
    /**
     * @brief Creates the system
     */
    Physics2D();

    /**
     * @brief Destroys the system
     */
    virtual ~Physics2D();

    /**
     * @brief Set the acceleration of gravity in the physics world
     *
     * @param gravity The acceleration from gravity in m/s
     */
    void setGravity(const glm::vec2& gravity);

    /**
     * @brief Returns the acceleration from gravity in m/s
     */
    const glm::vec2& getGravity() const { return gravity; }

    /**
     * @brief Sets the scale factor to convert world units to Box2D meters. Default is 1
     *
     * @param worldToBoxScale The new scale factor to use when syncing back to Transform2D
     */
    void setLengthUnitScale(float worldToBoxScale);

    /**
     * @brief Returns the scale factor to convert world units to Box2D meters
     */
    float getWorldToBoxScale() const { return worldToBoxScale; }

private:
    float worldToBoxScale;
    glm::vec2 gravity;
    b2WorldId worldId;

    virtual void init(engine::Engine&) override;
    virtual void update(std::mutex&, float dt, float, float, float) override;
};

} // namespace sys
} // namespace bl

#endif
