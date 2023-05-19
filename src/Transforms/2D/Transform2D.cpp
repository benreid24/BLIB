#include <BLIB/Transforms/2D/Transform2D.hpp>

#include <glm/gtx/transform.hpp>

namespace bl
{
namespace t2d
{
Transform2D::Transform2D()
: position(0.f, 0.f)
, scaleFactors(1.f, 1.f)
, rotation(0.f)
, depth(0.f) {}

Transform2D::Transform2D(const glm::vec2& pos)
: position(pos)
, scaleFactors(1.f, 1.f)
, rotation(0.f)
, depth(0.f) {}

void Transform2D::setPosition(const glm::vec2& pos) {
    position = pos;
    markDirty();
}

void Transform2D::move(const glm::vec2& delta) {
    position += delta;
    markDirty();
}

const glm::vec2& Transform2D::getPosition() const { return position; }

void Transform2D::setDepth(float d) {
    depth = d;
    markDirty();
}

void Transform2D::addDepth(float delta) {
    depth += delta;
    markDirty();
}

float Transform2D::getDepth() const { return depth; }

void Transform2D::setScale(const glm::vec2& factors) {
    scaleFactors = factors;
    markDirty();
}

void Transform2D::setScale(float factor) {
    scaleFactors.x = factor;
    scaleFactors.y = factor;
    markDirty();
}

void Transform2D::scale(const glm::vec2& factors) {
    scaleFactors.x *= factors.x;
    scaleFactors.y *= factors.y;
    markDirty();
}

void Transform2D::scale(float factor) {
    scaleFactors.x *= factor;
    scaleFactors.y *= factor;
    markDirty();
}

const glm::vec2& Transform2D::getScale() const { return scaleFactors; }

void Transform2D::setRotation(float angle) {
    rotation = angle;
    markDirty();
}

void Transform2D::rotate(float delta) {
    rotation += delta;
    markDirty();
}

float Transform2D::getRotation() const { return rotation; }

void Transform2D::refreshDescriptor(glm::mat4& dest) {
    dest = glm::translate(glm::vec3(position, depth));
    dest *= glm::rotate(glm::radians(rotation), glm::vec3(0.f, 0.f, -1.f)); // +/-?
    dest *= glm::scale(glm::vec3(scaleFactors, 1.f));
}

} // namespace t2d
} // namespace bl
