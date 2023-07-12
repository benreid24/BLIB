#ifndef BLIB_RENDER_CAMERAS_2D_CAMERAAFFECTOR2D_HPP
#define BLIB_RENDER_CAMERAS_2D_CAMERAAFFECTOR2D_HPP

#include <glm/glm.hpp>

namespace bl
{
namespace rc
{
namespace c2d
{
/**
 * @brief Base class for affectors that can be applied to 2d cameras. An affecter modifies the
 *        behavior of the camera without modifying the underlying camera. This allows affectors to
 *        be removed without having to worry about undoing their changes
 *
 * @ingroup Renderer
 */
struct CameraAffector2D {
    /**
     * @brief Destroys the affector
     */
    virtual ~CameraAffector2D() = default;

    /**
     * @brief Updates the affector
     *
     * @param dt Time elapsed, in seconds
     * @param recomputeView Set to true to make the camera recompute its view matrix
     * @param recomputeProj Set to true to make the camera recompute its projection matrix
     */
    virtual void update(float dt, bool& recomputeView, bool& recomputeProj);

    /**
     * @brief Base method does nothing. Use this to make changes to the parameters used to construct
     *        the camera's view matrix
     *
     * @param rotation The rotation of the camera
     */
    virtual void applyOnView(float& rotation);

    /**
     * @brief Base method does nothing. Use this to make changes to the parameters used to construct
     *        the camera's projection matrix
     *
     * @param center The center of the camera. May be modified
     * @param size The size of the region the camera is showing
     * @param rotation The rotation of the camera
     */
    virtual void applyOnProj(glm::vec2& center, glm::vec2& size);
};

} // namespace c2d
} // namespace rc
} // namespace bl

#endif
