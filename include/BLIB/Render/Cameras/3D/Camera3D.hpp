#ifndef BLIB_RENDER_CAMERAS_3D_CAMERA3D_HPP
#define BLIB_RENDER_CAMERAS_3D_CAMERA3D_HPP

#include <BLIB/Render/Cameras/3D/CameraController3D.hpp>
#include <BLIB/Render/Cameras/Camera.hpp>
#include <memory>

namespace bl
{
namespace render
{
namespace r3d
{
/**
 * @brief Camera for 3d scenes
 *
 * @ingroup Renderer
 */
class Camera3D : public Camera {
public:
    /**
     * @brief Create a new Camera
     *
     * @param pos The position of the camera in world space
     * @param yaw The yaw angle of the camera in degrees
     * @param pitch The pitch angle of the camera in degrees
     * @param fov The field of view of the camera
     */
    Camera3D(const glm::vec3& pos = {}, float yaw = 0.f, float pitch = 0.f, float fov = 75.f);

    /**
     * @brief Creates a new camera
     *
     * @param pos The position of the camera
     * @param lookAt The position the camera should look at
     * @param fov The field of view of the camera
     */
    Camera3D(const glm::vec3& pos, const glm::vec3& lookAt, float fov = 75.f);

    /**
     * @brief Destroys the camera
     */
    virtual ~Camera3D() = default;

    /**
     * @brief Set the world position of the camera
     *
     * @param pos The position of the camera in world space
     */
    void setPosition(const glm::vec3& pos);

    /**
     * @brief Orients the camera to look at the given position from its current position
     *
     * @param pos The position to look at
     */
    void lookAt(const glm::vec3& pos);

    /**
     * @brief Applies an offset to the camera's position
     *
     * @param offset World space units to add to the position
     */
    void move(const glm::vec3& offset);

    /**
     * @brief Returns the position of the camera in world space
     */
    constexpr const glm::vec3& getPosition() const;

    /**
     * @brief Sets the pitch angle of the camera
     *
     * @param pitch The pitch of the camera in degrees
     */
    void setPitch(float pitch);

    /**
     * @brief Adds the given offset to the pitch of the camera
     *
     * @param offset Offset to apply in degrees
     */
    void applyPitch(float offset);

    /**
     * @brief Returns the pitch of the camera in degrees
     */
    constexpr float getPitch() const;

    /**
     * @brief Sets the yaw angle of the camera
     *
     * @param yaw The yaw of the camera in degrees
     */
    void setYaw(float yaw);

    /**
     * @brief Adds the given offset to the yaw of the camera
     *
     * @param offset Offset to apply in degrees
     */
    void applyYaw(float offset);

    /**
     * @brief Returns the yaw of the camera in degrees
     */
    constexpr float getYaw() const;

    /**
     * @brief Set the FOV of the camera
     *
     * @param fov The FOV of the camera in degrees
     */
    void setFov(float fov);

    /**
     * @brief Returns the FOV of the camera
     */
    constexpr float getFov() const;

    /**
     * @brief Returns a normalized direction vector that the camera is facing
     */
    glm::vec3 getDirection() const;

    /**
     * @brief Sets the controller of this camera, freeing the previous controller, if any
     *
     * @tparam TController The type of controller to create. Must inherit from CameraController3D
     * @tparam ...TArgs Argument types to the controller constructor
     * @param ...args Arguments to the controller constructor
     * @return Pointer to the newly created controller
     */
    template<typename TController, typename... TArgs>
    TController* setController(TArgs&&... args);

    /**
     * @brief Sets the controller of this camera, freeing the previous controller, if any
     *
     * @tparam TController The type of controller to add
     * @param controller Pointer to the controller to add
     * @return Pointer to the controller
     */
    template<typename TController>
    TController* setController(std::unique_ptr<TController>&& controller);

private:
    glm::vec3 position;
    float pitch, yaw;
    float fov;

    std::unique_ptr<CameraController3D> controller;
    // TODO -  affectors

    virtual void refreshProjMatrix(glm::mat4& proj, const VkViewport& viewport) override;
    virtual void refreshViewMatrix(glm::mat4& view) override;
    virtual void update(float dt) override;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

inline constexpr const glm::vec3& Camera3D::getPosition() const { return position; }

inline constexpr float Camera3D::getPitch() const { return pitch; }

inline constexpr float Camera3D::getYaw() const { return yaw; }

inline constexpr float Camera3D::getFov() const { return fov; }

template<typename TController, typename... TArgs>
TController* Camera3D::setController(TArgs&&... args) {
    TController* c = new TController(std::forward<TArgs>(args)...);
    controller.reset(c);
    controller->setCam(*this);
    return c;
}

template<typename TController>
TController* Camera3D::setController(std::unique_ptr<TController>&& c) {
    TController* cr = c.get();
    controller.release();
    controller.swap(c);
    controller->setCam(*this);
    return c;
}

} // namespace r3d
} // namespace render
} // namespace bl

#endif
