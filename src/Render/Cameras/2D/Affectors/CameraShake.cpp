#include <BLIB/Render/Cameras/2D/Affectors/CameraShake.hpp>

#include <BLIB/Math.hpp>
#include <cmath>

namespace bl
{
namespace rc
{
namespace c2d
{
CameraShake::CameraShake(float m, float s)
: time(0.f)
, mag(m) {
    setShakesPerSecond(s);
}

void CameraShake::setMagnitude(float m) { mag = m; }

void CameraShake::setShakesPerSecond(float s) { speed = 2.f * math::Pi * s; }

void CameraShake::applyOnProj(glm::vec2& pos, glm::vec2&) {
    const float t = time * speed;

    pos.x += mag * std::cos(t);
    pos.y -= mag * std::cos(t);
}

void CameraShake::update(float dt, bool&, bool& proj) {
    time += dt;
    proj = true;
}

} // namespace c2d
} // namespace rc
} // namespace bl
