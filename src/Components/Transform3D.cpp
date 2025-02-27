#include <BLIB/Components/Transform3D.hpp>

#include <BLIB/Render/Vulkan/VulkanState.hpp>
#include <glm/gtx/transform.hpp>

namespace bl
{
namespace com
{
Transform3D::Transform3D()
: position(0.f, 0.f, 0.f)
, rotation(glm::identity<glm::quat>())
, scaleFactors(1.f, 1.f, 1.f) {}

void Transform3D::setPosition(const glm::vec3& pos) {
    position = pos;
    markDirty();
}

void Transform3D::move(const glm::vec3& offset) {
    position += offset;
    markDirty();
}

void Transform3D::setScale(const glm::vec3& s) {
    scaleFactors = s;
    markDirty();
}

void Transform3D::scale(const glm::vec3& f) {
    scaleFactors.x *= f.x;
    scaleFactors.y *= f.y;
    scaleFactors.z *= f.z;
    markDirty();
}

void Transform3D::setRotationEulerAngles(const glm::vec3& euler) {
    rotation = glm::quat(glm::radians(euler));
}

void Transform3D::rotate(const glm::vec3& axis, float angle) {
    glm::quat rot = glm::angleAxis(glm::radians(angle), glm::normalize(axis));
    rotation      = rot * rotation;
    markDirty();
}

void Transform3D::lookAt(const glm::vec3& pos, const glm::vec3& up) {
    glm::vec3 forward = glm::normalize(pos - position);
    glm::vec3 right   = glm::normalize(glm::cross(up, forward));
    glm::vec3 newUp   = glm::cross(forward, right);

    glm::mat3 rotationMatrix(right, newUp, forward);
    rotation = glm::quat_cast(rotationMatrix);

    markDirty();
}

glm::vec3 Transform3D::getForwardDir() const { return rotation * glm::vec3(0.f, 0.f, -1.f); }

glm::vec3 Transform3D::getRightDir() const { return rotation * glm::vec3(1.f, 0.f, 0.f); }

glm::vec3 Transform3D::getUpDir() const { return rotation * glm::vec3(0.f, 1.f, 0.f); }

void Transform3D::refreshDescriptor(rc::ds::Transform3DPayload& dest) {
    glm::mat4 transform = glm::translate(position);
    transform *= glm::scale(scaleFactors);
    transform *= glm::toMat4(rotation);
    dest = transform;
}

} // namespace com
} // namespace bl
