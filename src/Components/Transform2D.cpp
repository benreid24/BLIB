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
    makeDirty();
}

void Transform2D::setPosition(const glm::vec2& pos) {
    position = pos;
    makeDirty();
}

void Transform2D::move(const glm::vec2& delta) {
    position += delta;
    makeDirty();
}

glm::vec2 Transform2D::getGlobalPosition() {
    glm::vec2 result = position;
    Transform2D* t   = this;
    while (t->hasParent()) {
        t = &t->getParent();
        result += t->getLocalPosition() - t->getOrigin() * t->getScale();
    }
    return result;
}

void Transform2D::setDepth(float d) {
    depth = d;
    makeDirty();
}

void Transform2D::addDepth(float delta) {
    depth += delta;
    makeDirty();
}

float Transform2D::getDepth() const { return depth; }

float Transform2D::getGlobalDepth() const {
    if (hasParent()) { return getParent().getGlobalDepth() + depth; }
    return depth;
}

void Transform2D::setScale(const glm::vec2& factors) {
    scaleFactors = factors;
    makeDirty();
}

void Transform2D::setScale(float factor) {
    scaleFactors.x = factor;
    scaleFactors.y = factor;
    makeDirty();
}

void Transform2D::scale(const glm::vec2& factors) {
    scaleFactors.x *= factors.x;
    scaleFactors.y *= factors.y;
    makeDirty();
}

void Transform2D::scale(float factor) {
    scaleFactors.x *= factor;
    scaleFactors.y *= factor;
    makeDirty();
}

void Transform2D::setRotation(float angle) {
    rotation = angle;
    makeDirty();
}

void Transform2D::rotate(float delta) {
    rotation += delta;
    makeDirty();
}

void Transform2D::refreshDescriptor(glm::mat4& dest) { dest = getGlobalTransform(); }

glm::mat4 Transform2D::createTransformMatrix(const glm::vec2& origin, const glm::vec3& position,
                                             const glm::vec2& scale, float rotation) {
    glm::mat4 result = glm::translate(position);
    result           = glm::scale(result, glm::vec3(scale, 1.f));
    result           = glm::rotate(result, glm::radians(rotation), rc::Config::Rotate2DAxis);
    result           = glm::translate(result, glm::vec3(-origin, 0.f));
    return result;
}

glm::mat4 Transform2D::getLocalTransform() const {
    return createTransformMatrix(
        origin, glm::vec3(position, getGlobalDepth()), scaleFactors, rotation);
}

const glm::mat4& Transform2D::getGlobalTransform() {
    ensureUpdated();
    return cachedGlobalTransform;
}

void Transform2D::makeDirty() {
    markDirty();
    incrementVersion();
}

void Transform2D::ensureUpdated() {
    if (ParentAwareVersioned::refreshRequired()) {
        markRefreshed();
        if (hasParent()) {
            // TODO - only parent position? how to anchor in case of scaled/rotated parent?
            //      - use parentTform*pos to compute anchor point?
            cachedGlobalTransform = createTransformMatrix(
                origin, glm::vec3(getGlobalPosition(), getGlobalDepth()), scaleFactors, rotation);
        }
        else { cachedGlobalTransform = getLocalTransform(); }
    }
}

} // namespace com
} // namespace bl
