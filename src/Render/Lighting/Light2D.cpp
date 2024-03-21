#include <BLIB/Render/Lighting/Light2D.hpp>

#include <BLIB/Logging.hpp>
#include <BLIB/Render/Descriptors/Builtin/Scene2DInstance.hpp>
#include <BLIB/Render/Lighting/Scene2DLighting.hpp>

namespace bl
{
namespace rc
{
namespace lgt
{
Light2D::Light2D()
: owner(nullptr)
, index(Scene2DLighting::MaxLightCount) {}

Light2D::Light2D(Scene2DLighting& owner, std::uint32_t i)
: owner(&owner)
, index(i) {}

bool Light2D::isValid() const {
    return owner != nullptr && index < Scene2DLighting::MaxLightCount &&
           owner->allocations[index] < Scene2DLighting::MaxLightCount;
}

void Light2D::removeFromScene() {
    if (isValid()) { owner->removeLight(*this); }
}

void Light2D::setColor(const glm::vec3& color) {
    if (isValid()) {
        glm::vec4& c = owner->lighting.lights[owner->allocations[index]].color;
        c            = glm::vec4(color, c.w);
    }
    else { BL_LOG_WARN << "Cannot set the color of an invalid light"; }
}

glm::vec3 Light2D::getColor() const {
    if (isValid()) {
        glm::vec4& c = owner->lighting.lights[owner->allocations[index]].color;
        return glm::vec3(c.x, c.y, c.z);
    }
    else {
        BL_LOG_WARN << "Cannot get the color of an invalid light";
        return glm::vec3(0.f);
    }
}

void Light2D::setRadius(float r) {
    if (isValid()) { owner->lighting.lights[owner->allocations[index]].color.z = r; }
    else { BL_LOG_WARN << "Cannot set the radius of an invalid light"; }
}

float Light2D::getRadius() const {
    if (isValid()) { return owner->lighting.lights[owner->allocations[index]].color.z; }
    else {
        BL_LOG_WARN << "Cannot get the radius of an invalid light";
        return 0.f;
    }
}

void Light2D::setPosition(const glm::vec2& pos) {
    if (isValid()) { owner->lighting.lights[owner->allocations[index]].position = pos; }
    else { BL_LOG_WARN << "Cannot set the position of an invalid light"; }
}

glm::vec2 Light2D::getPosition() const {
    if (isValid()) { return owner->lighting.lights[owner->allocations[index]].position; }
    else {
        BL_LOG_WARN << "Cannot get the position of an invalid light";
        return glm::vec2(0.f);
    }
}

} // namespace lgt
} // namespace rc
} // namespace bl
