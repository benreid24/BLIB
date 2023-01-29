#include <BLIB/Render/Cameras/3D/Controllers/OrbiterController.hpp>

#include <BLIB/Math.hpp>
#include <BLIB/Render/Cameras/3D/Camera3D.hpp>
#include <cmath>

namespace bl
{
namespace render
{
namespace r3d
{
OrbiterController::OrbiterController(const glm::vec3& center, float p, const glm::vec3& normal,
                                     float minRadius, float maxRadius)
: center(center)
, time(0.f) {
    setNormal(normal);
    setRadius(minRadius, maxRadius);
    setPeriod(p);
}

void OrbiterController::setCenter(const glm::vec3& c) { center = c; }

void OrbiterController::setNormal(const glm::vec3& normal) {
    const glm::vec3 notParallel{-normal.y, normal.z, normal.x};
    a = glm::normalize(glm::cross(normal, notParallel));
    b = glm::normalize(glm::cross(normal, a));
}

void OrbiterController::setRadius(float close, float far) {
    if (far < 0.f) far = close;
    radiusCenter = (close + far) * 0.5f;
    radiusFluc   = (far - close) * 0.5f;
}

void OrbiterController::setPeriod(float p) {
    period        = p;
    periodInverse = 1.f / p;
}

void OrbiterController::update(float dt) {
    time += dt;
    if (time >= period) { time -= period; }

    const float radians = math::Pi * 2.f * (time * periodInverse);
    const float cos     = std::cos(radians);
    const float sin     = std::sin(radians);

    const float r      = radiusCenter + cos * radiusFluc;
    const glm::vec3 v1 = r * cos * a;
    const glm::vec3 v2 = r * sin * b;

    camera().setPosition(center + v1 + v2);
    camera().lookAt(center);
}

} // namespace r3d
} // namespace render
} // namespace bl
