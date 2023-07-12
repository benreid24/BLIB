#ifndef BLIB_RENDER_CAMERAS_3D_CONTROLLERS_ORBITERCONTROLER_HPP
#define BLIB_RENDER_CAMERAS_3D_CONTROLLERS_ORBITERCONTROLER_HPP

#include <BLIB/Render/Cameras/3D/CameraController3D.hpp>
#include <glm/glm.hpp>

namespace bl
{
namespace rc
{
namespace c3d
{
/**
 * @brief Basic controller for 3d cameras that orbits a center point
 *
 * @ingroup Renderer
 */
class OrbiterController : public CameraController3D {
public:
    /**
     * @brief Creates the controller with the given orbital parameters
     *
     * @param center The position to orbit around
     * @param period Time in seconds to complete an orbit in
     * @param normal Vector to circle around
     * @param minRadius Smallest radius of the orbit
     * @param maxRadius Largest radius of the orbit
     */
    OrbiterController(const glm::vec3& center, float period, const glm::vec3& normal,
                      float minRadius, float maxRadius = -1.f);

    /**
     * @brief Destroys the controller
     */
    virtual ~OrbiterController() = default;

    /**
     * @brief Sets the center point of the orbit
     *
     * @param center The new position to orbit around
     */
    void setCenter(const glm::vec3& center);

    /**
     * @brief Sets the normal vector to circle around
     *
     * @param normal The new normal vector of the orbital plane
     */
    void setNormal(const glm::vec3& normal);

    /**
     * @brief Sets the radii of the orbit
     *
     * @param minRadius Smallest radius of the orbit
     * @param maxRadius Largest radius of the orbit
     */
    void setRadius(float minRadius, float maxRadius = -1.f);

    /**
     * @brief Sets the period, in seconds, of the orbit
     *
     * @param period The time to take to complete one revolution
     */
    void setPeriod(float period);

    /**
     * @brief Updates the camera by progressing the orbit by dt seconds
     *
     * @param dt Time in seconds to move along the orbit in
     */
    virtual void update(float dt) override;

private:
    glm::vec3 center;
    glm::vec3 a, b;
    float radiusCenter, radiusFluc;
    float period, periodInverse;
    float time;
};

} // namespace c3d
} // namespace rc
} // namespace bl

#endif
