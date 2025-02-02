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
    if (system) { b2Body_ApplyForceToCenter(bodyId, {force.x, force.y}, true); }
}

void Physics2D::applyImpulseToCenter(const glm::vec2& impulse) {
    if (system) { b2Body_ApplyLinearImpulseToCenter(bodyId, {impulse.x, impulse.y}, true); }
}

void Physics2D::teleport(const glm::vec2& position, float angle, bool cv) {
    if (system) {
        transform->setPosition(position);
        transform->setRotation(angle);

        if (cv) {
            b2Body_SetLinearVelocity(bodyId, {0.f, 0.f});
            b2Body_SetAngularVelocity(bodyId, 0.f);
        }

        const float scale = system->getWorldToBoxScale(entity);
        b2Body_SetTransform(bodyId,
                            {position.x * scale, position.y * scale},
                            b2MakeRot(math::degreesToRadians(angle)));
        b2Body_SetAwake(bodyId, true);
    }
}

void Physics2D::setVelocity(const glm::vec2& v) {
    if (system) { b2Body_SetLinearVelocity(bodyId, {v.x, v.y}); }
}

void Physics2D::setLinearDamping(float d) {
    if (system) { b2Body_SetLinearDamping(bodyId, d); }
}

float Physics2D::getMass() const { return system ? b2Body_GetMass(bodyId) : 0.f; }

float Physics2D::scaleWorldToPhysics(float worldDistance) {
    if (!system) { return worldDistance; }
    return worldDistance * system->getWorldToBoxScale(entity);
}

float Physics2D::scalePhysicToWorld(float physics) {
    if (!system) { return physics; }
    return physics / system->getWorldToBoxScale(entity);
}

void Physics2D::clampLinearVelocity(float maxVelocity) {
    if (system) {
        maxVelocity         = scaleWorldToPhysics(maxVelocity);
        auto vel            = b2Body_GetLinearVelocity(bodyId);
        const float magSqrd = vel.x * vel.x + vel.y * vel.y;
        if (magSqrd > maxVelocity * maxVelocity) {
            const float mag = std::sqrt(magSqrd);
            vel.x           = vel.x / mag * maxVelocity;
            vel.y           = vel.y / mag * maxVelocity;
            b2Body_SetLinearVelocity(bodyId, vel);
        }
    }
}

glm::vec2 Physics2D::getLinearVelocity() const {
    if (system) {
        const auto vel = b2Body_GetLinearVelocity(bodyId);
        return {vel.x, vel.y};
    }
    return {0.f, 0.f};
}

} // namespace com
} // namespace bl
