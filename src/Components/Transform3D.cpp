#include <BLIB/Components/Transform3D.hpp>

#include <BLIB/Render/Vulkan/VulkanState.hpp>
#include <glm/gtx/transform.hpp>

namespace bl
{
namespace com
{
Transform3D::Transform3D()
: position(0.f, 0.f, 0.f)
, orientation()
, scaleFactors(1.f, 1.f, 1.f) {}

void Transform3D::setPosition(const glm::vec3& pos) {
    position = pos;
    markDirty();
}

void Transform3D::move(const glm::vec3& offset) {
    position += offset;
    markDirty();
}

Orientation3D& Transform3D::getOrientationForChange() {
    markDirty();
    return orientation;
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

void Transform3D::refreshDescriptor(rc::ds::Transform3DPayload& dest) {
    glm::mat4 transform = glm::translate(position);
    transform *= glm::rotate(glm::radians(orientation.getRoll()), orientation.getFaceDirection());
    transform *= glm::scale(scaleFactors);
    dest = transform;
}

} // namespace com
} // namespace bl
