#include <BLIB/Objects/Components/Camera3D.hpp>

#include <BLIB/Engine/Engine.hpp>

namespace bl
{
namespace obj
{
namespace bcom
{
Camera3D::Camera3D()
: camera(nullptr) {}

void Camera3D::attachCamera(const glm::vec3& offset, const glm::vec3& localFaceDir, float fov,
                            float roll) {
    if (!camera) {
        camera = engine().ecs().emplaceComponent<com::Camera3D>(entity(), engine(), entity(), fov);
    }

    camera->getTransform().setPosition(offset);
    camera->getTransform().getOrientationForChange().setFaceDirection(localFaceDir);
    camera->getTransform().getOrientationForChange().setRoll(roll);
    camera->setFOV(fov);
}

void Camera3D::removeCamera() {
    if (camera) {
        engine().ecs().removeComponent<com::Camera3D>(entity());
        camera = nullptr;
    }
}

void Camera3D::makeCameraActive(unsigned int player) {
    if (camera) { camera->makeActive(player); }
}

void Camera3D::makeCameraInactive(unsigned int player) {
    if (camera) { camera->makeInactive(player); }
}

} // namespace bcom
} // namespace obj
} // namespace bl
