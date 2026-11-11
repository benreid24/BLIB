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
     * @brief Sets the roll angle of the camera
     *
     * @param roll The roll in degrees
     */
    void setRoll(float roll);

    /**
     * @brief Sets the local position of the camera relative to the entity's transform
     *
     * @param position The local position to set
     */
    void setLocalPosition(const glm::vec3& position);

    /**
     * @brief Returns the local position of the camera relative to the entity's transform
     */
    const glm::vec3& getLocalPosition() const { return localPosition; }

    /**
     * @brief Sets the local forward direction of the camera relative to the entity's transform
     *
     * @param dir The local forward direction to set
     */
    void setLocalForwardDirection(const glm::vec3& dir);

    /**
     * @brief Returns the local forward direction of the camera relative to the entity's transform
     */
    const glm::vec3& getLocalForwardDirection() const { return localDirection; }

    /**
     * @brief Computes and returns the world position of the camera
     */
    glm::vec3 getWorldPosition() const;

    /**
     * @brief Computes and returns the forward direction of the camera
     */
    glm::vec3 getWorldForwardDirection() const;

    /**
     * @brief Returns the field of view in degrees
     */
    float getFOV() const { return fov; }

    /**
     * @brief Returns the roll angle in degrees
     */
    float getRoll() const { return roll; }

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
    glm::vec3 localPosition;
    glm::vec3 localDirection;
    float fov;
    float roll;
};

} // namespace com
} // namespace bl

#endif
