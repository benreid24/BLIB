#include <BLIB/Components/Transform3D.hpp>

#include <BLIB/Render/Vulkan/VulkanState.hpp>
#include <glm/gtx/matrix_decompose.hpp>
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
    makeDirty();
}

void Transform3D::move(const glm::vec3& offset) {
    position += offset;
    makeDirty();
}

void Transform3D::setScale(const glm::vec3& s) {
    scaleFactors = s;
    makeDirty();
}

void Transform3D::scale(const glm::vec3& f) {
    scaleFactors.x *= f.x;
    scaleFactors.y *= f.y;
    scaleFactors.z *= f.z;
    makeDirty();
}

void Transform3D::setRotationEulerAngles(const glm::vec3& euler) {
    rotation = glm::quat(glm::radians(euler));
}

void Transform3D::rotate(const glm::vec3& axis, float angle) {
    glm::quat rot = glm::angleAxis(glm::radians(angle), glm::normalize(axis));
    rotation      = rot * rotation;
    makeDirty();
}

void Transform3D::lookAt(const glm::vec3& pos, const glm::vec3& worldUp) {
    glm::vec3 forward = glm::normalize(pos - position);

    glm::vec3 up = worldUp;
    if (glm::abs(glm::dot(forward, worldUp)) > 0.999f) {
        up = glm::abs(forward.y) > 0.999f ? glm::vec3(1, 0, 0) : glm::vec3(0, 1, 0);
    }

    glm::vec3 right = glm::normalize(glm::cross(up, forward));
    glm::vec3 newUp = glm::cross(forward, right);

    glm::mat3 rotationMatrix(right, newUp, forward);
    rotation = glm::quat_cast(rotationMatrix);

    makeDirty();
}

glm::vec3 Transform3D::getForwardDir() const { return rotation * glm::vec3(0.f, 0.f, 1.f); }

glm::vec3 Transform3D::getRightDir() const { return rotation * glm::vec3(1.f, 0.f, 0.f); }

glm::vec3 Transform3D::getUpDir() const { return rotation * glm::vec3(0.f, 1.f, 0.f); }

void Transform3D::refreshDescriptor(rc::dsi::Transform3DPayload& dest) {
    dest = getGlobalTransform();
}

void Transform3D::makeDirty() {
    if (markDirty()) { incrementVersion(); }
}

glm::mat4 Transform3D::getLocalTransform() const {
    glm::mat4 transform = glm::translate(position);
    transform *= glm::scale(scaleFactors);
    transform *= glm::toMat4(rotation);
    return transform;
}

glm::mat4 Transform3D::getGlobalTransform() const {
    if (hasParent()) { return getParent().getGlobalTransform() * getLocalTransform(); }
    return getLocalTransform();
}

glm::mat4 Transform3D::getGlobalTransformExcludingRoot() const {
    if (hasParent() && getParent().hasParent()) {
        return getParent().getGlobalTransformExcludingRoot() * getLocalTransform();
    }
    return getLocalTransform();
}

glm::vec3 Transform3D::transformPoint(const glm::vec3& point) const {
    return glm::vec3(getGlobalTransform() * glm::vec4(point, 1.f));
}

void Transform3D::setTransform(const glm::mat4& mat) {
    glm::vec3 skew;
    glm::vec4 perspective;
    glm::decompose(mat, scaleFactors, rotation, position, skew, perspective);
    makeDirty();
}

} // namespace com
} // namespace bl
