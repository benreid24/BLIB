#ifndef BLIB_RENDER_CAMERAS_2D_CAMERACONTROLLER2D_HPP
#define BLIB_RENDER_CAMERAS_2D_CAMERACONTROLLER2D_HPP

namespace bl
{
namespace rc
{
namespace c2d
{
class Camera2D;

/**
 * @brief Base class for 2d camera controllers
 *
 * @ingroup Renderer
 */
struct CameraController2D {
    /**
     * @brief Destroy the controller
     */
    virtual ~CameraController2D() = default;

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
    constexpr Camera2D& camera();

private:
    Camera2D* cam;

    void setCam(Camera2D& cam);

    friend class Camera2D;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

inline constexpr Camera2D& CameraController2D::camera() { return *cam; }

inline void CameraController2D::setCam(Camera2D& c) { cam = &c; }

} // namespace c2d
} // namespace rc
} // namespace bl

#endif
