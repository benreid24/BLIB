#ifndef BLIB_CAMERAS_OVERLAYCAMERA_HPP
#define BLIB_CAMERAS_OVERLAYCAMERA_HPP

#include <BLIB/Cameras/Camera.hpp>

namespace bl
{
namespace cam
{
/**
 * @brief Special camera that provides a fixed view for overlays
 *
 * @ingroup Renderer
 */
class OverlayCamera : public Camera {
public:
    /**
     * @brief Creates the overlay camera
     */
    OverlayCamera();

    /**
     * @brief Destroys the overlay camera
     */
    virtual ~OverlayCamera() = default;

private:
    virtual void update(float dt) override;
    virtual void refreshViewMatrix(glm::mat4& view) override;
    virtual void refreshProjMatrix(glm::mat4& proj, const VkViewport& viewport) override;
};

} // namespace cam
} // namespace bl

#endif
