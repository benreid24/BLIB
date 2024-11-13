#ifndef BLIB_SYSTEMS_PHYSICS2D_HPP
#define BLIB_SYSTEMS_PHYSICS2D_HPP

#include <BLIB/Components/Physics2D.hpp>
#include <BLIB/ECS/Events.hpp>
#include <BLIB/Engine/Events/Worlds.hpp>
#include <BLIB/Engine/System.hpp>
#include <BLIB/Events/Listener.hpp>
#include <box2d/box2d.h>
#include <glm/glm.hpp>
#include <optional>

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
     * @brief Event that is fired when an entity enters a sensor
     */
    struct SensorEntered {
        ecs::Entity entity;
        ecs::Entity sensor;
    };

    /**
     * @brief Event that is fired when an entity exits a sensor
     */
    struct SensorExited {
        ecs::Entity entity;
        ecs::Entity sensor;
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
     * @brief Queries the world and finds the entity whose collision shape(s) contain the given
     *        position
     *
     * @param world The world to query
     * @param pos The position to query for
     * @param filter Optional Box2D filter to use during the world query
     * @return The physics component of the entity at the given position. May be nullptr
     */
    com::Physics2D* findEntityAtPosition(const engine::World2D& world, const glm::vec2& pos,
                                         b2QueryFilter filter = b2DefaultQueryFilter()) const;

    /**
     * @brief Creates a physics sensor for the given entity. Must have a Transform2D and Hitbox2D
     *
     * @param entity The entity to create a sensor on
     * @param filter Optional collision filter for the sensor
     */
    void createSensorForEntity(ecs::Entity entity, b2Filter filter = b2DefaultFilter());

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

    /**
     * @brief Helper method to get the physics component from a Box2D shape id
     *
     * @param shapeId The shape to get the physics component for
     * @return Pointer to the physics component. May be nullptr (but should not be)
     */
    static com::Physics2D* getPhysicsComponentFromShape(b2ShapeId shapeId);

    /**
     * @brief Helper method to get an entity from a Box2D shape id
     *
     * @param shapeId The shape to get the entity for
     * @return The entity the shape belongs to
     */
    static ecs::Entity getEntityFromShape(b2ShapeId shapeId);

private:
    engine::Engine* engine;
    std::vector<engine::World2D*> worlds;

    virtual void init(engine::Engine&) override;
    virtual void update(std::mutex&, float dt, float, float, float) override;
    virtual void observe(const ecs::event::ComponentRemoved<com::Physics2D>& event) override;
    virtual void observe(const engine::event::WorldCreated& event) override;
    virtual void observe(const engine::event::WorldDestroyed& event) override;

    std::optional<b2BodyId> createBody(
        b2BodyDef& bodyDef, b2ShapeDef& shapeDef,
        ecs::ComponentSet<ecs::Require<com::Hitbox2D, com::Transform2D>>& components);
};

} // namespace sys
} // namespace bl

#endif
