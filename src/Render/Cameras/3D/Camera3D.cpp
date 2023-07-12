#include <BLIB/Render/Cameras/3D/Camera3D.hpp>

#include <BLIB/Math/Trig.hpp>
#include <BLIB/Render/Config.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

namespace bl
{
namespace rc
{
namespace c3d
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
    glm::vec3 pos             = position;
    t3d::Orientation3D orient = orientation;
    for (auto& a : affectors) { a->applyOnView(pos, orient); }

    // roll by rotating up vector around look vector
    const glm::vec3 up =
        glm::mat3(glm::rotate(glm::radians(orient.getRoll()), orient.getFaceDirection())) *
        Config::UpDirection;

    view = glm::lookAt(pos, pos + orient.getFaceDirection(), up);
}

void Camera3D::refreshProjMatrix(glm::mat4& proj, const VkViewport& viewport) {
    float f         = fov;
    float nearValue = nearPlane();
    float farValue  = farPlane();
    for (auto& a : affectors) { a->applyOnProjection(f, nearValue, farValue); }

    proj = glm::perspective(glm::radians(f), viewport.width / viewport.height, nearValue, farValue);
    proj[1][1] *= -1.f;
}

void Camera3D::update(float dt) {
    if (controller) { controller->update(dt); }
    bool view = false;
    bool proj = false;
    for (auto& a : affectors) { a->update(dt, view, proj); }
    if (view) { markViewDirty(); }
    if (proj) { markProjDirty(); }
}

void Camera3D::removeAffector(const CameraAffector3D* a) {
    for (auto it = affectors.begin(); it != affectors.end(); ++it) {
        if (static_cast<CameraAffector3D*>(it->get()) == a) {
            affectors.erase(it);
            break;
        }
    }
}

} // namespace c3d
} // namespace rc
} // namespace bl
