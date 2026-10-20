#ifndef BLIB_ENGINE_WORLDS_WORLD3D_HPP
#define BLIB_ENGINE_WORLDS_WORLD3D_HPP

#include <BLIB/Components/Navmesh.hpp>
#include <BLIB/ECS/Events.hpp>
#include <BLIB/Engine/Worlds/BasicWorld.hpp>
#include <BLIB/Render/Scenes/Scene3D.hpp>
#include <BLIB/Signals/Listener.hpp>

namespace bl
{
namespace engine
{
/**
 * @brief World type for 3d worlds with physics and collisions
 *
 * @ingroup Engine
 */
class World3D
: public BasicWorld<rc::scene::Scene3D>
, sig::Listener<ecs::event::ComponentAdded<com::Navmesh>,
                ecs::event::ComponentRemoved<com::Navmesh>> {
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

    /**
     * @brief Convenience function to get the navmesh component for this world if one was created
     */
    com::Navmesh* getNavmesh() const { return navmesh; }

protected:
    com::Navmesh* navmesh;

    virtual void process(const ecs::event::ComponentAdded<com::Navmesh>& signal) override;
    virtual void process(const ecs::event::ComponentRemoved<com::Navmesh>& signal) override;
};

} // namespace engine
} // namespace bl

#endif
