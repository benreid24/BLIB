#ifndef BLIB_ENGINE_WORLD_HPP
#define BLIB_ENGINE_WORLD_HPP

#include <BLIB/ECS/Registry.hpp>
#include <BLIB/Render/Resources/SceneRef.hpp>

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
class World {
public:
    /**
     * @brief Destroys the world
     */
    virtual ~World() = default;

    /**
     * @brief Returns the ECS registry for this world
     */
    ecs::Registry& ecs() { return entities; }

    /**
     * @brief Returns the scene for this world
     */
    const rc::SceneRef& scene() const { return renderScene; }

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
    World(rc::SceneRef renderScene)
    : renderScene(renderScene) {}

private:
    ecs::Registry entities;
    rc::SceneRef renderScene;
    unsigned int index;

    friend class Engine;
};

} // namespace engine
} // namespace bl

#endif
