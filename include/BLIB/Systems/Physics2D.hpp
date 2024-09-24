#ifndef BLIB_SYSTEMS_PHYSICS2D_HPP
#define BLIB_SYSTEMS_PHYSICS2D_HPP

#include <BLIB/Components/Physics2D.hpp>
#include <BLIB/ECS/Events.hpp>
#include <BLIB/Engine/Events/Worlds.hpp>
#include <BLIB/Engine/System.hpp>
#include <BLIB/Events/Listener.hpp>
#include <box2d/box2d.h>
#include <glm/glm.hpp>

namespace bl
{
namespace engine
{
class World2D;
}
namespace sys
{
/**
 * @brief Physics system for 2d physics. Wraps Box2D and adapts the ECS to it
 *
 * @ingroup Systems
 */
class Physics2D
: public engine::System
, public event::Listener<ecs::event::ComponentRemoved<com::Physics2D>, engine::event::WorldCreated,
                         engine::event::WorldDestroyed> {
public:
    /**
     * @brief Event that is fired when an entity collision begins
     */
    struct EntityCollisionBeginEvent {
        ecs::Entity entityA;
        ecs::Entity entityB;
    };

    /**
     * @brief Event that is fired when an entity collision ends
     */
    struct EntityCollisionEndEvent {
        ecs::Entity entityA;
        ecs::Entity entityB;
    };

    /**
     * @brief Event that is fired when an entity hit event occurs
     */
    struct EntityCollisionHitEvent {
        ecs::Entity entityA;
        ecs::Entity entityB;
        b2ContactHitEvent& source;
    };

    /**
     * @brief Creates the system
     */
    Physics2D();

    /**
     * @brief Destroys the system
     */
    virtual ~Physics2D() = default;

    /**
     * @brief Adds physics simulation to the given entity. The entity must have a Transform2D and
     *        Hitbox2D component
     *
     * @param entity The entity to add physics to
     * @param bodyDef The body parameters for Box2D
     * @param shapeDef The shape parameters for Box2D
     * @return The created physics component, or nullptr on error
     */
    com::Physics2D* addPhysicsToEntity(ecs::Entity entity, b2BodyDef bodyDef = b2DefaultBodyDef(),
                                       b2ShapeDef shapeDef = b2DefaultShapeDef());

    /**
     * @brief Returns the scale from world coordinates to Box2D coordinates for the given world
     *
     * @param worldIndex The world index to get the scale for
     * @return The scale mapping world space to box2d space
     */
    float getWorldToBoxScale(unsigned int worldIndex) const;

    /**
     * @brief Returns the scale from world coordinates to Box2D coordinates for the given entity
     *
     * @param worldIndex The entity to get the scale for
     * @return The scale mapping world space to box2d space
     */
    float getWorldToBoxScale(ecs::Entity entity) const;

private:
    engine::Engine* engine;
    std::vector<engine::World2D*> worlds;

    virtual void init(engine::Engine&) override;
    virtual void update(std::mutex&, float dt, float, float, float) override;
    virtual void observe(const ecs::event::ComponentRemoved<com::Physics2D>& event) override;
    virtual void observe(const engine::event::WorldCreated& event) override;
    virtual void observe(const engine::event::WorldDestroyed& event) override;
};

} // namespace sys
} // namespace bl

#endif
