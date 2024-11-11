#ifndef BLIB_ENGINE_WORLD_HPP
#define BLIB_ENGINE_WORLD_HPP

#include <BLIB/ECS/Entity.hpp>
#include <BLIB/ECS/Flags.hpp>
#include <BLIB/Render/Resources/SceneRef.hpp>
#include <BLIB/Util/NonCopyable.hpp>

namespace bl
{
namespace engine
{
class Engine;

/**
 * @brief Base class for engine worlds. A world is a scene and all the entities in it. Players can
 *        be in worlds. Only derived world classes may be used
 *
 * @ingroup Engine
 */
class World : private util::NonCopyable {
public:
    static constexpr unsigned int MaxWorlds = 8;

    /**
     * @brief Destroys the world
     */
    virtual ~World();

    /**
     * @brief Creates an entity in this world
     *
     * @param flags The flags to create the entity with
     * @return The new entity
     */
    ecs::Entity createEntity(ecs::Flags flags = ecs::Flags::None);

    /**
     * @brief Destroys all entities in this world
     */
    void destroyAllEntities();

    /**
     * @brief Returns the game engine instance
     */
    Engine& engine() const { return owner; }

    /**
     * @brief Returns the scene for this world
     */
    rc::SceneRef scene() const { return renderScene; }

    /**
     * @brief Returns the index of this world
     */
    unsigned int worldIndex() const { return index; }

protected:
    /**
     * @brief Creates the world
     *
     * @param renderScene The scene for the world
     */
    World(Engine& owner, rc::SceneRef renderScene)
    : owner(owner)
    , renderScene(renderScene) {}

private:
    Engine& owner;
    rc::SceneRef renderScene;
    unsigned int index;

    friend class Engine;
};

} // namespace engine
} // namespace bl

#endif
