#ifndef BLIB_RENDER_CAMERAS_3D_AFFECTORS_HPP
#define BLIB_RENDER_CAMERAS_3D_AFFECTORS_HPP

#include <BLIB/Render/Cameras/3D/CameraAffector3D.hpp>

namespace bl
{
namespace gfx
{
namespace c3d
{
/**
 * @brief Camera affector that shakes the view
 *
 * @ingroup Renderer
 */
class CameraShake : public CameraAffector3D {
public:
    /**
     * @brief Creates a new camera shake affector
     *
     * @param magnitude The magnitude of the camera shake
     * @param shakesPerSecond The speed of the camera shake
     */
    CameraShake(float magnitude, float shakesPerSecond);

    /**
     * @brief Destroys the camera affector
     */
    virtual ~CameraShake() = default;

    /**
     * @brief Sets the magnitude of the camera shake
     *
     * @param magnitude The magnitude to shake with
     */
    void setMagnitude(float magnitude);

    /**
     * @brief Returns the magnitude of the camera shake
     */
    constexpr float getMagnitude() const;

    /**
     * @brief Sets the speed of the camera shake
     *
     * @param shakesPerSecond How many shakes per second to apply
     */
    void setShakesPerSecond(float shakesPerSecond);

    /**
     * @brief Modifies the camera view matrix with the shake
     *
     * @param position Camera position to modify
     * @param orientation Ignored
     */
    virtual void applyOnView(glm::vec3& position, t3d::Orientation3D& orientation) override;

    /**
     * @brief Updates the camera shaking
     *
     * @param dt Time elapsed in seconds
     * @param view Flag to set to true to mark the view matrix dirty
     * @param proj Ignored
     */
    virtual void update(float dt, bool& view, bool& proj);

private:
    float time;
    float mag;
    float speed;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

inline constexpr float CameraShake::getMagnitude() const { return mag; }

} // namespace c3d
} // namespace gfx
} // namespace bl

#endif
