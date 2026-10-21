#ifndef BLIB_SYSTEMS_PATHFINDINGSYSTEM3D_HPP
#define BLIB_SYSTEMS_PATHFINDINGSYSTEM3D_HPP

#include <BLIB/Components/Navmesh.hpp>
#include <BLIB/Components/NavmeshPhysicsPather.hpp>
#include <BLIB/ECS.hpp>
#include <BLIB/Engine/System.hpp>
#include <BLIB/Signals/Listener.hpp>
#include <vector>

namespace bl
{
namespace sys
{
/**
 * @brief System for performing pathfinding in 3d worlds
 *
 * @ingroup Systems
 */
class PathfindingSystem3D
: public engine::System
, public sig::Listener<ecs::event::ComponentAdded<com::Navmesh>,
                       ecs::event::ComponentRemoved<com::Navmesh>> {
public:
    /**
     * @brief Creates the system
     */
    PathfindingSystem3D();

    /**
     * @brief Destroys the system
     */
    virtual ~PathfindingSystem3D() = default;

private:
    struct WorldNavmeshes {
        com::Navmesh* navmesh = nullptr;
    };

    ecs::ComponentPool<com::NavmeshPhysicsPather>* physicsPathers;
    std::vector<WorldNavmeshes> navmeshes;

    virtual void init(engine::Engine& engine) override;
    virtual void update(std::mutex& stageMutex, float dt, float realDt, float residual,
                        float realResidual) override;

    virtual void process(const ecs::event::ComponentAdded<com::Navmesh>& signal) override;
    virtual void process(const ecs::event::ComponentRemoved<com::Navmesh>& signal) override;
};

} // namespace sys
} // namespace bl

#endif
