#ifndef BLIB_RENDER_CAMERAS_3D_CONTROLLERS_ORBITERCONTROLER_HPP
#define BLIB_RENDER_CAMERAS_3D_CONTROLLERS_ORBITERCONTROLER_HPP

#include <BLIB/Render/Cameras/3D/CameraController3D.hpp>
#include <glm/glm.hpp>

namespace bl
{
namespace render
{
namespace r3d
{
class OrbiterController : public CameraController3D {
public:
    OrbiterController(const glm::vec3& center, float period, const glm::vec3& normal,
                      float minRadius, float maxRadius = -1.f);

    void setCenter(const glm::vec3& center);

    void setNormal(const glm::vec3& normal);

    void setRadius(float minRadius, float maxRadius = -1.f);

    void setPeriod(float period);

    virtual void update(float dt) override;

private:
    glm::vec3 center;
    glm::vec3 a, b;
    float radiusCenter, radiusFluc;
    float period, periodInverse;
    float time;
};

} // namespace r3d
} // namespace render
} // namespace bl

#endif
