#ifndef BLIB_RENDER_CAMERAS_CAMERA_HPP
#define BLIB_RENDER_CAMERAS_CAMERA_HPP

#include <glad/vulkan.h>
#include <glm/glm.hpp>

namespace bl
{
namespace render
{
/**
 * @brief Base class for all cameras
 *
 * @ingroup Renderer
 */
class Camera {
public:
    /**
     * @brief Destroy the Camera object
     */
    virtual ~Camera() = default;

    /**
     * @brief Updates the camera
     *
     * @param dt Time elapsed since last call to update
     */
    virtual void update(float dt) = 0;

    /**
     * @brief Returns the view matrix of this camera
     */
    const glm::mat4& getViewMatrix();

    /**
     * @brief Returns the projection matrix of this camera for the given viewport
     *
     * @param viewport The viewport to project to
     * @return The projection matrix to use
     */
    const glm::mat4& getProjectionMatrix(const VkViewport& viewport);

    /**
     * @brief Sets the near plane distance
     * 
     * @param near The near plane distance
    */
    void setNearPlane(float near);

    /**
     * @brief Sets the far plane distance
     *
     * @param far The far plane distance
     */
    void setFarPlane(float far);

    /**
     * @brief Sets the distance to the near and far planes
     * 
     * @param near Distance to the near plane
     * @param far Distance to the far plane
    */
    void setNearAndFarPlanes(float near, float far);

protected:
    /**
     * @brief Construct a new Camera
     */
    Camera();

    /**
     * @brief Mark the view matrix as needing to be refreshed on next use
     */
    void markViewDirty();

    /**
     * @brief Marks the projection matrix as needing to be refreshed on next use
     */
    void markProjDirty();

    /**
     * @brief Called when the view matrix needs to be refreshed
     *
     * @param view Reference to the matrix to populate
     */
    virtual void refreshViewMatrix(glm::mat4& view) = 0;

    /**
     * @brief Called when the projection matrix needs to be refreshed
     *
     * @param proj Reference to the matrix to populate
     * @param viewport The viewport to project to
     */
    virtual void refreshProjMatrix(glm::mat4& proj, const VkViewport& viewport) = 0;

    /**
     * @brief Returns the distance to the near plane
     */
    constexpr float nearPlane() const;

    /**
     * @brief Returns the distance to the far plane
     */
    constexpr float farPlane() const;

private:
    glm::mat4 view;
    bool viewDirty;

    glm::mat4 proj;
    bool projDirty;
    float vpWidth, vpHeight;
    float near, far;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

inline constexpr float Camera::nearPlane() const { return near; }

inline constexpr float Camera::farPlane() const { return far; }

} // namespace render
} // namespace bl

#endif
