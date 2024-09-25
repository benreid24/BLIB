#include <BLIB/Components/Physics2D.hpp>

#include <BLIB/Math.hpp>
#include <BLIB/Systems/Physics2D.hpp>

namespace bl
{
namespace com
{
Physics2D::Physics2D(sys::Physics2D& system, ecs::Entity owner, Transform2D& transform,
                     b2BodyId bodyId)
: system(&system)
, entity(owner)
, transform(&transform)
, bodyId(bodyId) {
    b2Body_SetUserData(bodyId, this);
}

void Physics2D::applyForceToCenter(const glm::vec2& force) {
    b2Body_ApplyForceToCenter(bodyId, {force.x, force.y}, true);
}

void Physics2D::applyImpulseToCenter(const glm::vec2& impulse) {
    b2Body_ApplyLinearImpulseToCenter(bodyId, {impulse.x, impulse.y}, true);
}

void Physics2D::teleport(const glm::vec2& position, float angle, bool cv) {
    transform->setPosition(position);
    transform->setRotation(angle);

    if (cv) {
        b2Body_SetLinearVelocity(bodyId, {0.f, 0.f});
        b2Body_SetAngularVelocity(bodyId, 0.f);
    }
    const float scale = system->getWorldToBoxScale(entity);
    b2Body_SetTransform(
        bodyId, {position.x * scale, position.y * scale}, b2MakeRot(math::degreesToRadians(angle)));
    b2Body_SetAwake(bodyId, true);
}

void Physics2D::setVelocity(const glm::vec2& v) { b2Body_SetLinearVelocity(bodyId, {v.x, v.y}); }

void Physics2D::setLinearDamping(float d) { b2Body_SetLinearDamping(bodyId, d); }

} // namespace com
} // namespace bl
