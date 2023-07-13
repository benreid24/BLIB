#ifndef BLIB_CAMERAS_3D_CAMERAAFFECTOR3D_HPP
#define BLIB_CAMERAS_3D_CAMERAAFFECTOR3D_HPP

#include <BLIB/Components/Orientation3D.hpp>
#include <glm/glm.hpp>

namespace bl
{
namespace cam
{
/**
 * @brief Base class for affectors that can be applied to 3d cameras. An affecter modifies the
 *        behavior of the camera without modifying the underlying camera. This allows affectors to
 *        be removed without having to worry about undoing their changes
 *
 * @ingroup Cameras
 */
struct CameraAffector3D {
    /**
     * @brief Destroys the affector
     */
    virtual ~CameraAffector3D() = default;

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
     * @param position The position of the camera. May be modified
     * @param orientation The orientation of the camera. May be modified
     */
    virtual void applyOnView(glm::vec3& position, com::Orientation3D& orientation);

    /**
     * @brief Base method does nothing. Use this to make changes to the parameters used to construct
     *        the camera's projection matrix
     *
     * @param fov The field of view of the camera in degrees. May be modified
     * @param near Distance of the near plane. May be modified
     * @param far Distance of the far plane. May be modified
     */
    virtual void applyOnProjection(float& fov, float& nearValue, float& farValue);
};

} // namespace cam
} // namespace bl

#endif
