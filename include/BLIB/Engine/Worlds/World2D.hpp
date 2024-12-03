#ifndef BLIB_ENGINE_WORLDS_WORLD2D_HPP
#define BLIB_ENGINE_WORLDS_WORLD2D_HPP

#include <BLIB/Components/Physics2D.hpp>
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
    /// Callback signature for world spatial queries. Return false to terminate the query
    using WorldQueryCallback = bool (*)(com::Physics2D*, void*);

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

    /**
     * @brief Queries the world and calls the callback for each entity within the rectangle
     *
     * @param upperBound The upper corner in world coordinates
     * @param lowerBound The lower corner in world coordinates
     * @param filter Box2D query filter to apply
     * @param callback The callback to call
     * @param ctx User provided data which is passed back to the callback
     */
    void queryAABB(glm::vec2 upperBound, glm::vec2 lowerBound, b2QueryFilter filter,
                   WorldQueryCallback callback, void* ctx) const;

private:
    float worldToBoxScale;
    glm::vec2 gravity;
    b2WorldId boxWorld;
};

} // namespace engine
} // namespace bl

#endif
