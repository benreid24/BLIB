#ifndef BLIB_RENDER_CAMERAS_OVERLAYCAMERA_HPP
#define BLIB_RENDER_CAMERAS_OVERLAYCAMERA_HPP

#include <BLIB/Render/Cameras/Camera.hpp>

namespace bl
{
namespace gfx
{
namespace ovy
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

} // namespace ovy
} // namespace gfx
} // namespace bl

#endif
