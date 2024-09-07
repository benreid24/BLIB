#ifndef BLIB_ENGINE_WORLDS_WORLD2D_HPP
#define BLIB_ENGINE_WORLDS_WORLD2D_HPP

#include <BLIB/Engine/Worlds/BasicWorld.hpp>
#include <BLIB/Render/Scenes/Scene2D.hpp>
#include <box2d/box2d.h>
#include <glm/glm.hpp>

namespace bl
{
namespace engine
{
/**
 * @brief World type for 2d worlds with physics and collisions
 *
 * @ingroup Engine
 */
class World2D : public BasicWorld<rc::scene::Scene2D> {
public:
    /**
     * @brief Creates the world
     *
     * @param owner The game engine instance
     */
    World2D(Engine& owner);

    /**
     * @brief Destroys the world
     */
    virtual ~World2D();

    /**
     * @brief Returns the Box2D world id for this world
     */
    b2WorldId getBox2dWorldId() const { return boxWorld; }

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
    b2WorldId boxWorld;
};

} // namespace engine
} // namespace bl

#endif
