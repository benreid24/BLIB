#ifndef BLIB_COMPONENTS_CAMERA3D_HPP
#define BLIB_COMPONENTS_CAMERA3D_HPP

#include <BLIB/Components/Transform3D.hpp>
#include <BLIB/ECS/Entity.hpp>

namespace bl
{
namespace engine
{
class Engine;
}

namespace com
{
/**
 * @brief Camera component that attaches to an entity and uses its transform
 *
 * @ingroup Components
 */
class Camera3D {
public:
    /**
     * @brief Creates the camera component
     *
     * @param engine The game engine instance
     * @param entity The entity that owns this component
     * @param fov The field of view of the camera in degrees
     */
    Camera3D(engine::Engine& world, ecs::Entity entity, float fov = 90.0f);

    /**
     * @brief Sets the field of view of the camera
     *
     * @param fov The fov in degrees
     */
    void setFOV(float fov);

    /**
     * @brief Returns the transform of the camera
     */
    Transform3D& getTransform() { return localTransform; }

    /**
     * @brief Returns the transform of the camera
     */
    const Transform3D& getTransform() const { return localTransform; }

    /**
     * @brief Returns the field of view in degrees
     */
    float getFOV() const { return fov; }

    /**
     * @brief Makes this camera the active camera for the given player
     *
     * @param playerIndex The player to make the camera active for
     * @return True if the camera was able to made active, false otherwise
     */
    bool makeActive(unsigned int playerIndex = 0);

    /**
     * @brief Makes the camera active for all players
     */
    void makeActiveForAllPlayers();

    /**
     * @brief Makes the camera inactive for the given player
     *
     * @param playerIndex The player to remove the camera from
     * @return True if the camera was previously active, false on noop
     */
    bool makeInactive(unsigned int playerIndex = 0);

    /**
     * @brief Removes the camera from all players
     */
    void makeInactiveForAllPlayers();

private:
    engine::Engine& engine;
    Transform3D* transform;
    Transform3D localTransform;
    glm::vec3 localPosition;
    float fov;
};

} // namespace com
} // namespace bl

#endif
