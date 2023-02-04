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
Camera3D::Camera3D(const glm::vec3& pos, float yaw, float pitch, float fov, float roll)
: position(pos)
, orientation(yaw, pitch, roll)
, fov(fov) {}

Camera3D::Camera3D(const glm::vec3& pos, const glm::vec3& look, float fov, float roll)
: position(pos)
, orientation(look, pos, roll)
, fov(fov) {}

void Camera3D::setPosition(const glm::vec3& pos) {
    position = pos;
    markViewDirty();
}

void Camera3D::move(const glm::vec3& offset) {
    position += offset;
    markViewDirty();
}

t3d::Orientation3D& Camera3D::getOrientationForChange() {
    markViewDirty();
    return orientation;
}

void Camera3D::setFov(float f) {
    fov = f;
    markProjDirty();
}

void Camera3D::refreshViewMatrix(glm::mat4& view) {
    // TODO - affectors
    view = glm::lookAt(position, position + orientation.getFaceDirection(), Config::UpDirection);
}

void Camera3D::refreshProjMatrix(glm::mat4& proj, const VkViewport& viewport) {
    // TODO - affectors
    proj = glm::perspective(
        glm::radians(fov), viewport.width / viewport.height, nearPlane(), farPlane());
}

void Camera3D::update(float dt) {
    if (controller) { controller->update(dt); }
    // TODO - affectors
}

} // namespace r3d
} // namespace render
} // namespace bl
