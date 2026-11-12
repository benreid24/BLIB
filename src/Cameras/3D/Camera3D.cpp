#include <BLIB/Cameras/3D/Camera3D.hpp>

#include <BLIB/Render/Config/Constants.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

namespace bl
{
namespace cam
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

com::Orientation3D& Camera3D::getOrientationForChange() {
    markViewDirty();
    return orientation;
}

void Camera3D::setFov(float f) {
    fov = f;
    markProjDirty();
}

void Camera3D::refreshViewMatrix(glm::mat4& view) {
    glm::vec3 pos             = position;
    com::Orientation3D orient = orientation;
    for (auto& a : affectors) { a->applyOnView(pos, orient); }

    const glm::vec3 forward = orient.getRotationQuaternion() * rc::cfg::Constants::ForwardDirection;
    const glm::vec3 up      = orient.getRotationQuaternion() * rc::cfg::Constants::UpDirection;

    view = glm::lookAt(pos, pos + forward, up);
}

void Camera3D::refreshProjMatrix(glm::mat4& proj, const VkViewport& viewport) {
    float f         = fov;
    float nearValue = nearPlane();
    float farValue  = farPlane();
    for (auto& a : affectors) { a->applyOnProjection(f, nearValue, farValue); }

    proj = glm::perspective(glm::radians(f), viewport.width / viewport.height, nearValue, farValue);
    proj[1][1] *= -1.f;

    cachedViewportSize.x = viewport.width;
    cachedViewportSize.y = viewport.height;
}

void Camera3D::update(float dt) {
    if (!controllers.empty()) { controllers.back()->update(dt); }
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

glm::vec2 Camera3D::getViewerSize() const { return cachedViewportSize; }

glm::vec3 Camera3D::getObserverPosition() const { return getPosition(); }

void Camera3D::popController() {
    if (!controllers.empty()) { controllers.pop_back(); }
}

void Camera3D::removeController(const CameraController3D* c) {
    for (auto it = controllers.begin(); it != controllers.end(); ++it) {
        if (static_cast<CameraController3D*>(it->get()) == c) {
            controllers.erase(it);
            break;
        }
    }
}

void Camera3D::removeAllControllers() { controllers.clear(); }

} // namespace cam
} // namespace bl
