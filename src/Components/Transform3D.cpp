#include <BLIB/Components/Transform3D.hpp>

#include <BLIB/Render/Vulkan/VulkanLayer.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/transform.hpp>

namespace bl
{
namespace com
{
Transform3D::Transform3D()
: position(0.f, 0.f, 0.f)
, orientation()
, scaleFactors(1.f, 1.f, 1.f) {}

Transform3D& Transform3D::operator=(const Transform3D& copy) {
    position     = copy.position;
    orientation  = copy.orientation;
    scaleFactors = copy.scaleFactors;
    makeDirty();
    return *this;
}

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

glm::vec3 Transform3D::getGlobalForwardDir() const {
    return getGlobalRotation() * rc::cfg::Constants::ForwardDirection;
}

glm::vec3 Transform3D::getGlobalRightDir() const {
    return getGlobalRotation() * rc::cfg::Constants::RightDirection;
}

glm::vec3 Transform3D::getGlobalUpDir() const {
    return getGlobalRotation() * rc::cfg::Constants::UpDirection;
}

void Transform3D::refreshDescriptor(rc::dsi::Transform3DPayload& dest) {
    dest = getGlobalTransform();
    markRefreshed();
}

void Transform3D::makeDirty() {
    if (markDirty()) { incrementVersion(); }
}

glm::mat4 Transform3D::getLocalTransform() const {
    return glm::translate(position) * glm::toMat4(orientation.getRotationQuaternion()) *
           glm::scale(scaleFactors);
}

glm::mat4 Transform3D::getGlobalTransform() const {
    if (hasParent()) { return getParent().getGlobalTransform() * getLocalTransform(); }
    return getLocalTransform();
}

glm::vec3 Transform3D::transformPoint(const glm::vec3& point) const {
    return glm::vec3(getGlobalTransform() * glm::vec4(point, 1.f));
}

void Transform3D::setTransform(const glm::mat4& mat) {
    glm::vec3 skew;
    glm::vec4 perspective;
    glm::quat rotation;
    glm::decompose(mat, scaleFactors, rotation, position, skew, perspective);
    orientation.setRotationQuaternion(rotation);
    makeDirty();
}

bool Transform3D::isDirty() const {
    if (ParentAwareVersioned::refreshRequired()) { return true; }
    return DescriptorComponentBase::isDirty();
}

void Transform3D::setParent(Transform3D& parent) {
    ParentAwareVersioned<Transform3D>::setParent(&parent);
    makeDirty();
}

glm::vec3 Transform3D::getGlobalPosition() const {
    if (hasParent()) {
        return glm::vec3(getParent().getGlobalTransform() * glm::vec4(position, 1.f));
    }
    return position;
}

glm::quat Transform3D::getGlobalRotation() const {
    if (hasParent()) {
        return getParent().getGlobalRotation() * orientation.getRotationQuaternion();
    }
    return orientation.getRotationQuaternion();
}

Orientation3D& Transform3D::getOrientationForChange() {
    makeDirty();
    return orientation;
}

} // namespace com
} // namespace bl
