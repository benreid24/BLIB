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

    /**
     * @brief Configures the width and height of the virtual coordinate space for all overlays. The
     *        engine configs "blib.overlay.width" and "blib.overlay.height" are used to populate the
     *        default if present, otherwise (1.f, 1.f) is used
     *
     * @param width The width in units
     * @param height The height in units
     */
    static void setOverlayCoordinateSpace(float width, float height);

private:
    unsigned int settingVersion;

    virtual void update(float dt) override;
    virtual void refreshViewMatrix(glm::mat4& view) override;
    virtual void refreshProjMatrix(glm::mat4& proj, const VkViewport& viewport) override;
};

} // namespace cam
} // namespace bl

#endif
