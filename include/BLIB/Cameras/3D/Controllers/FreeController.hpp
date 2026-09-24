#ifndef BLIB_CAMERAS_3D_CONTROLLERS_FREECONTROLLER_HPP
#define BLIB_CAMERAS_3D_CONTROLLERS_FREECONTROLLER_HPP

#include <BLIB/Cameras/3D/CameraController3D.hpp>
#include <glm/glm.hpp>

namespace bl
{
namespace cam
{
namespace c3d
{
/**
 * @brief Freeform camera controller that allows flying around
 *
 * @ingroup Cameras
 */
class FreeController : public CameraController3D {
public:
    /**
     * @brief Creates the camera controller
     *
     * @param maxSpeed The maximum speed of the camera in world units per second
     * @param acceleration The acceleration of the camera in world units per second squared
     * @param timeToStop The time it takes for the camera to stop moving in seconds
     */
    FreeController(float maxSpeed, float acceleration, float timeToStop = 0.2f);

    /**
     * @brief Destroys the camera controller
     */
    virtual ~FreeController() = default;

    /**
     * @brief Applies a yaw movement with the given delta in the next update
     *
     * @param delta The delta to apply, positive is right, negative is left
     */
    void applyYaw(float delta);

    /**
     * @brief Applies a pitch movement with the given delta in the next update
     *
     * @param delta The delta to apply, positive is up, negative is down
     */
    void applyPitch(float delta);

    /**
     * @brief Applies a roll movement with the given delta in the next update
     *
     * @param delta The delta to apply, positive is right, negative is left
     */
    void applyRoll(float delta);

    /**
     * @brief Applies a forward movement with the given factor in the next update
     *
     * @param factor The factor to apply, positive is forward, negative is backward
     */
    void moveForward(float factor);

    /**
     * @brief Applies a right movement with the given factor in the next update
     *
     * @param factor The factor to apply, positive is right, negative is left
     */
    void moveRight(float factor);

    /**
     * @brief Applies an up movement with the given factor in the next update
     *
     * @param factor The factor to apply, positive is up, negative is down
     */
    void moveUp(float factor);

    /**
     * @brief Updates the camera with the given delta time
     *
     * @param dt Elapsed time in seconds
     */
    virtual void update(float dt) override;

private:
    glm::vec3 velocity; // in local space
    glm::vec3 appliedMovement;
    glm::vec3 appliedRotation;
    float dampening;
    float acceleration;
    float maxSpeed;
};

} // namespace c3d
} // namespace cam
} // namespace bl

#endif
