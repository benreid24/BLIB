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

Camera3D::Camera3D(const glm::vec3& pos, const glm::vec3& look, float fov)
: position(pos)
, fov(fov) {
    lookAt(look);
}

void Camera3D::setPosition(const glm::vec3& pos) {
    position = pos;
    markViewDirty();
}

void Camera3D::lookAt(const glm::vec3& pos) {
    const glm::vec3 dir = glm::normalize(pos - position); // correct order?
    setYaw(glm::degrees(std::atan2(dir.z, dir.x)));
    setPitch(glm::degrees(std::asin(dir.y)));
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
    const float yawRad   = glm::radians(yaw);
    const float pitchRad = glm::radians(pitch);
    return glm::normalize(glm::vec3{std::cos(yawRad) * std::cos(pitchRad),
                                    std::sin(pitchRad),
                                    std::sin(yawRad) * std::cos(pitchRad)});
}

void Camera3D::refreshViewMatrix(glm::mat4& view) {
    view = glm::lookAt(position, position + getDirection(), Config::UpDirection);
}

void Camera3D::refreshProjMatrix(glm::mat4& proj, const VkViewport& viewport) {
    proj = glm::perspective(
        glm::radians(fov), viewport.width / viewport.height, viewport.minDepth, viewport.maxDepth);
}

void Camera3D::update(float dt) {
    if (controller) { controller->update(dt); }
    // TODO - affectors
}

} // namespace r3d
} // namespace render
} // namespace bl
