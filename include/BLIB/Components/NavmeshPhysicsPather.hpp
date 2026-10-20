#ifndef BLIB_COMPONENTS_NAVMESHPHYSICSPATHER_HPP
#define BLIB_COMPONENTS_NAVMESHPHYSICSPATHER_HPP

#include <BLIB/ECS/Entity.hpp>
#include <DetourNavMeshQuery.h>
#include <glm/glm.hpp>
#include <vector>

namespace bl
{
namespace ecs
{
class Registry;
}
namespace engine
{
class World3D;
}

namespace com
{
class Navmesh;
class Physics3DCharacter;
class Transform3D;

/**
 * @brief Component for pathing a 3d entity along a navmesh using physics
 *
 * @ingroup Components
 */
class NavmeshPhysicsPather {
public:
    /**
     * @brief Creates the pather for the given entity in the world
     *
     * @param world The world the entity is in
     * @param entity The entity to create the pather for
     */
    NavmeshPhysicsPather(engine::World3D& world, ecs::Entity entity);

    /**
     * @brief Creates the pather from the underlying components required
     *
     * @param navmesh The navmesh the entity is pathing on
     * @param transform The transform of the entity
     * @param character The physics character of the entity
     */
    NavmeshPhysicsPather(const Navmesh& navmesh, const Transform3D& transform,
                         Physics3DCharacter& character);

    /**
     * @brief Finds the path to the given destination and stores it
     *
     * @param destination The position to path to
     * @return True if a path was found, false if not
     */
    bool findPath(const glm::vec3& destination);

    /**
     * @brief Performs pathfinding if the entity is currently pathing
     *
     * @param dt Elapsed engine time in seconds
     */
    void update(float dt);

private:
    const Navmesh* navmesh;
    const Transform3D* transform;
    Physics3DCharacter* character;
    std::vector<dtPolyRef> path;
    unsigned int currentPoly;
};

} // namespace com
} // namespace bl

#endif
