#include <BLIB/Transforms/3D/Orientation3D.hpp>

namespace bl
{
namespace t3d
{
Orientation3D::Orientation3D(float yaw, float pitch, float roll)
: roll(roll) {
    setYawAndPitch(yaw, pitch);
}

Orientation3D::Orientation3D(const glm::vec3& dir, float r)
: faceDir(dir)
, roll(r) {}

Orientation3D::Orientation3D(const glm::vec3& lookAt, const glm::vec3& from, float r)
: faceDir(glm::normalize(lookAt - from))
, roll(r) {}

void Orientation3D::setPitch(float p) { setYawAndPitch(getYaw(), p); }

void Orientation3D::applyPitch(float offset) { setYawAndPitch(getYaw(), getPitch() + offset); }

float Orientation3D::getPitch() const { return glm::degrees(std::asin(faceDir.y)); }

void Orientation3D::setYaw(float y) { setYawAndPitch(y, getPitch()); }

void Orientation3D::applyYaw(float offset) { setYawAndPitch(getYaw(), getPitch() + offset); }

float Orientation3D::getYaw() const { return glm::degrees(std::atan2(faceDir.z, faceDir.x)); }

void Orientation3D::setRoll(float r) { roll = r; }

void Orientation3D::applyRoll(float r) { roll += r; }

void Orientation3D::lookAt(const glm::vec3& pos, const glm::vec3& from) {
    faceDir = glm::normalize(pos - from);
}

void Orientation3D::setYawAndPitch(float yaw, float pitch) {
    const float yawRad   = glm::radians(yaw);
    const float pitchRad = glm::radians(pitch);
    faceDir              = glm::normalize(glm::vec3{std::cos(yawRad) * std::cos(pitchRad),
                                       std::sin(pitchRad),
                                       std::sin(yawRad) * std::cos(pitchRad)});
}

void Orientation3D::setFaceDirection(const glm::vec3& f) { faceDir = f; }

} // namespace t3d
} // namespace bl
