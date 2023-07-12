#ifndef BLIB_RENDER_CAMERAS_2D_AFFECTORS_HPP
#define BLIB_RENDER_CAMERAS_2D_AFFECTORS_HPP

#include <BLIB/Render/Cameras/2D/CameraAffector2D.hpp>

namespace bl
{
namespace rc
{
namespace c2d
{
/**
 * @brief Camera affector that shakes the view
 *
 * @ingroup Renderer
 */
class CameraShake : public CameraAffector2D {
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
     * @param center The center of the camera. May be modified
     * @param size The size of the region the camera is showing
     */
    virtual void applyOnProj(glm::vec2& center, glm::vec2& size) override;

    /**
     * @brief Updates the camera shaking
     *
     * @param dt Time elapsed in seconds
     * @param view Flag to set to true to mark the view matrix dirty
     * @param view Flag to set to true to mark the projection matrix dirty
     */
    virtual void update(float dt, bool& view, bool& proj) override;

private:
    float time;
    float mag;
    float speed;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

inline constexpr float CameraShake::getMagnitude() const { return mag; }

} // namespace c2d
} // namespace rc
} // namespace bl

#endif
