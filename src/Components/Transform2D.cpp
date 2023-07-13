#include <BLIB/Components/Transform2D.hpp>

#include <BLIB/Render/Config.hpp>
#include <BLIB/Render/Vulkan/VulkanState.hpp>
#include <glm/gtx/transform.hpp>

namespace bl
{
namespace com
{
Transform2D::Transform2D()
: origin(0.f, 0.f)
, position(0.f, 0.f)
, scaleFactors(1.f, 1.f)
, rotation(0.f)
, depth(0.f) {}

Transform2D::Transform2D(const glm::vec2& pos)
: origin(0.f, 0.f)
, position(pos)
, scaleFactors(1.f, 1.f)
, rotation(0.f)
, depth(0.f) {}

void Transform2D::setOrigin(const glm::vec2& o) {
    origin = o;
    markDirty();
}

void Transform2D::setPosition(const glm::vec2& pos) {
    position = pos;
    markDirty();
}

void Transform2D::move(const glm::vec2& delta) {
    position += delta;
    markDirty();
}

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

void Transform2D::setRotation(float angle) {
    rotation = angle;
    markDirty();
}

void Transform2D::rotate(float delta) {
    rotation += delta;
    markDirty();
}

void Transform2D::refreshDescriptor(glm::mat4& dest) { dest = getMatrix(); }

glm::mat4 Transform2D::getMatrix() const {
    glm::mat4 result = glm::translate(glm::vec3(position, depth));
    result           = glm::scale(result, glm::vec3(scaleFactors, 1.f));
    result           = glm::rotate(result, glm::radians(rotation), rc::Config::Rotate2DAxis);
    result           = glm::translate(result, glm::vec3(-origin, 0.f));
    return result;
}

glm::mat4 Transform2D::getInverse() const { return glm::inverse(getMatrix()); }

} // namespace com
} // namespace bl
