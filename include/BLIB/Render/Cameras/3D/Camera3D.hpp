#ifndef BLIB_RENDER_CAMERAS_3D_CAMERA3D_HPP
#define BLIB_RENDER_CAMERAS_3D_CAMERA3D_HPP

#include <BLIB/Render/Cameras/3D/CameraController3D.hpp>
#include <BLIB/Render/Cameras/Camera.hpp>
#include <BLIB/Transforms/3D/Orientation3D.hpp>
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
    Camera3D(const glm::vec3& pos = {}, float yaw = 0.f, float pitch = 0.f, float fov = 75.f,
             float roll = 0.f);

    /**
     * @brief Creates a new camera
     *
     * @param pos The position of the camera
     * @param lookAt The position the camera should look at
     * @param fov The field of view of the camera
     * @param roll The roll of the camera
     */
    Camera3D(const glm::vec3& pos, const glm::vec3& lookAt, float fov = 75.f, float roll = 0.f);

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
     * @brief Returns the orientation of the camera
     */
    constexpr const t3d::Orientation3D& getOrientation() const;

    /**
     * @brief Returns the orientation of the camera and marks the camera as needing to refresh the
     *        view matrix. Use this method to change the orientation of the camera
     */
    t3d::Orientation3D& getOrientationForChange();

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
    t3d::Orientation3D orientation;
    float fov;

    std::unique_ptr<CameraController3D> controller;
    // TODO -  affectors

    virtual void refreshProjMatrix(glm::mat4& proj, const VkViewport& viewport) override;
    virtual void refreshViewMatrix(glm::mat4& view) override;
    virtual void update(float dt) override;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

inline constexpr const glm::vec3& Camera3D::getPosition() const { return position; }

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

inline constexpr const t3d::Orientation3D& Camera3D::getOrientation() const { return orientation; }

} // namespace r3d
} // namespace render
} // namespace bl

#endif
