#include <BLIB/Render/Cameras/3D/Affectors/CameraShake.hpp>

#include <BLIB/Math.hpp>
#include <cmath>

namespace bl
{
namespace gfx
{
namespace c3d
{
CameraShake::CameraShake(float m, float s)
: time(0.f)
, mag(m) {
    setShakesPerSecond(s);
}

void CameraShake::setMagnitude(float m) { mag = m; }

void CameraShake::setShakesPerSecond(float s) { speed = 2.f * math::Pi * s; }

void CameraShake::applyOnView(glm::vec3& pos, t3d::Orientation3D&) {
    const float t = time * speed;

    pos.x += mag * std::cos(t);
    pos.y += mag * std::cos(t);
    pos.z += mag * std::cos(-t);
}

void CameraShake::update(float dt, bool& view, bool&) {
    time += dt;
    view = true;
}

} // namespace c3d
} // namespace gfx
} // namespace bl
