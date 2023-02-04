#include <BLIB/Transforms/3D/Transform3D.hpp>

#include <glm/gtx/transform.hpp>

namespace bl
{
namespace t3d
{
Transform3D::Transform3D()
: position(0.f, 0.f, 0.f)
, orientation()
, scaleFactors(1.f, 1.f, 1.f)
, dirty(true) {}

void Transform3D::setPosition(const glm::vec3& pos) {
    position = pos;
    dirty    = true;
}

void Transform3D::move(const glm::vec3& offset) {
    position += offset;
    dirty = true;
}

Orientation3D& Transform3D::getOrientationForChange() {
    dirty = true;
    return orientation;
}

void Transform3D::setScale(const glm::vec3& s) {
    scaleFactors = s;
    dirty        = true;
}

void Transform3D::scale(const glm::vec3& f) {
    scaleFactors.x *= f.x;
    scaleFactors.y *= f.y;
    scaleFactors.z *= f.z;
    dirty = true;
}

void Transform3D::computeTransform(glm::mat4& result) {
    result = glm::translate(position);
    result *= glm::rotate(glm::radians(orientation.getRoll()), orientation.getFaceDirection());
    result *= glm::scale(scaleFactors);
    dirty = false;
}

} // namespace t3d
} // namespace bl
