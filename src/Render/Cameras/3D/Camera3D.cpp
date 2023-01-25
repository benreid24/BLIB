#include <BLIB/Render/Cameras/3D/Camera3D.hpp>

#include <BLIB/Math/Trig.hpp>
#include <BLIB/Render/Config.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace bl
{
namespace render
{
namespace r3d
{
Camera3D::Camera3D(const glm::vec3& pos, float yaw, float pitch, float fov)
: position(pos)
, pitch(pitch)
, yaw(yaw)
, fov(fov) {}

void Camera3D::setPosition(const glm::vec3& pos) {
    position = pos;
    markViewDirty();
}

void Camera3D::move(const glm::vec3& offset) {
    position += offset;
    markViewDirty();
}

void Camera3D::setPitch(float p) {
    pitch = p;
    markViewDirty();
}

void Camera3D::applyPitch(float offset) {
    pitch += offset;
    markViewDirty();
}

void Camera3D::setYaw(float y) {
    yaw = y;
    markViewDirty();
}

void Camera3D::applyYaw(float offset) {
    yaw += offset;
    markViewDirty();
}

void Camera3D::setFov(float f) {
    fov = f;
    markProjDirty();
}

glm::vec3 Camera3D::getDirection() const {
    // TODO - should we just cache this?
    return glm::normalize(glm::vec3{
        math::cos(yaw) * math::cos(pitch), math::sin(pitch), math::sin(yaw) * math::cos(pitch)});
}

void Camera3D::refreshViewMatrix(glm::mat4& view) {
    view = glm::lookAt(position, position + getDirection(), Config::UpDirection);
}

void Camera3D::refreshProjMatrix(glm::mat4& proj, const VkViewport& viewport) {
    // TODO - where to get far value?
    proj = glm::perspective(glm::radians(fov), viewport.width / viewport.height, 0.1f, 10.f);
}

} // namespace r3d
} // namespace render
} // namespace bl
