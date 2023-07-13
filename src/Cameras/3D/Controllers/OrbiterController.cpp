#include <BLIB/Cameras/3D/Controllers/OrbiterController.hpp>

#include <BLIB/Cameras/3D/Camera3D.hpp>
#include <BLIB/Math.hpp>
#include <cmath>

namespace bl
{
namespace cam
{
namespace c3d
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

void OrbiterController::setRadius(float close, float farValue) {
    if (farValue < 0.f) farValue = close;
    radiusCenter = (close + farValue) * 0.5f;
    radiusFluc   = (farValue - close) * 0.5f;
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

    const float r          = radiusCenter + cos * radiusFluc;
    const glm::vec3 v1     = r * cos * a;
    const glm::vec3 v2     = r * sin * b;
    const glm::vec3 newPos = center + v1 + v2;

    camera().setPosition(newPos);
    camera().getOrientationForChange().lookAt(center, newPos);
}

} // namespace c3d
} // namespace cam
} // namespace bl
