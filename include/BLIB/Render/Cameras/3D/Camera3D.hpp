#ifndef BLIB_RENDER_CAMERAS_3D_CAMERA3D_HPP
#define BLIB_RENDER_CAMERAS_3D_CAMERA3D_HPP

#include <BLIB/Render/Cameras/Camera.hpp>

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

private:
    glm::vec3 position;
    float pitch, yaw;
    float fov;
    // TODO - controller + affectors

    virtual void refreshProjMatrix(glm::mat4& proj, const VkViewport& viewport) override;
    virtual void refreshViewMatrix(glm::mat4& view) override;
    virtual void update(float dt) override;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

inline constexpr const glm::vec3& Camera3D::getPosition() const { return position; }

inline constexpr float Camera3D::getPitch() const { return pitch; }

inline constexpr float Camera3D::getYaw() const { return yaw; }

inline constexpr float Camera3D::getFov() const { return fov; }

} // namespace r3d
} // namespace render
} // namespace bl

#endif
