#ifndef BLIB_CAMERAS_3D_CAMERACONTROLLER3D_HPP
#define BLIB_CAMERAS_3D_CAMERACONTROLLER3D_HPP

namespace bl
{
namespace cam
{
class Camera3D;

/**
 * @brief Base class for 3d camera controllers
 *
 * @ingroup Cameras
 */
struct CameraController3D {
    /**
     * @brief Destroy the controller
     */
    virtual ~CameraController3D() = default;

    /**
     * @brief Override this to provide functionality to the camera
     *
     * @param dt Time elapsed in seconds
     */
    virtual void update(float dt) = 0;

protected:
    /**
     * @brief Returns the camera that owns this controller. Undefined behavior if not currently
     * owned
     */
    constexpr Camera3D& camera();

private:
    Camera3D* cam;

    void setCam(Camera3D& cam);

    friend class Camera3D;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

inline constexpr Camera3D& CameraController3D::camera() { return *cam; }

inline void CameraController3D::setCam(Camera3D& c) { cam = &c; }

} // namespace cam
} // namespace bl

#endif
