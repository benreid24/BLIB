#ifndef BLIB_ENGINE_WORLDS_WORLD3D_HPP
#define BLIB_ENGINE_WORLDS_WORLD3D_HPP

#include <BLIB/Engine/Worlds/BasicWorld.hpp>
#include <BLIB/Render/Scenes/Scene3D.hpp>

namespace bl
{
namespace engine
{
/**
 * @brief World type for 3d worlds with physics and collisions
 *
 * @ingroup Engine
 */
class World3D : public BasicWorld<rc::scene::Scene3D> {
public:
    /**
     * @brief Creates the world
     *
     * @param owner The game engine instance
     */
    World3D(Engine& owner);

    /**
     * @brief Destroys the world
     */
    virtual ~World3D();
};

} // namespace engine
} // namespace bl

#endif
