#ifndef BLIB_ENGINE_WORLD_HPP
#define BLIB_ENGINE_WORLD_HPP

#include <BLIB/ECS/Registry.hpp>
#include <BLIB/Render/Resources/SceneRef.hpp>

namespace bl
{
namespace engine
{
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

protected:
    /**
     * @brief Creates the world
     *
     * @param scene The scene for the world
     */
    World(rc::SceneRef scene)
    : scene(scene) {}

private:
    ecs::Registry entities;
    rc::SceneRef scene;
};

} // namespace engine
} // namespace bl

#endif
