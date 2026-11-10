#include <BLIB/Components/Physics3DCharacter.hpp>

#include <BLIB/Components/Transform3D.hpp>
#include <BLIB/Engine/Worlds/World3D.hpp>

namespace bl
{
namespace com
{
namespace
{
JPH::RVec3 jvec(const glm::vec3& v) { return JPH::RVec3(v.x, v.y, v.z); }

JPH::Quat jquat(const glm::quat& q) { return JPH::Quat(q.x, q.y, q.z, q.w); }
} // namespace

Physics3DCharacter::Physics3DCharacter(ecs::Entity entity, engine::World3D& world,
                                       const JPH::CharacterSettings& s)
: Physics3DCharacter(entity, world, world.engine().ecs().getComponent<Transform3D>(entity), s) {}

void Physics3DCharacter::setLinearVelocity(const glm::vec3& velocity) {
    character.SetLinearVelocity(jvec(velocity));
}

void Physics3DCharacter::addLinearVelocity(const glm::vec3& velocity) {
    character.AddLinearVelocity(jvec(velocity));
}

void Physics3DCharacter::addImpulse(const glm::vec3& impulse) {
    character.AddImpulse(jvec(impulse));
}

void Physics3DCharacter::setPosition(const glm::vec3& position) {
    character.SetPosition(jvec(position));
}

void Physics3DCharacter::setRotation(const glm::quat& rotation) {
    character.SetRotation(jquat(rotation));
}

void Physics3DCharacter::setPositionAndRotation(const glm::vec3& position,
                                                const glm::quat& rotation) {
    character.SetPositionAndRotation(jvec(position), jquat(rotation));
}

bool Physics3DCharacter::setShape(JPH::Ref<JPH::Shape> shape, float maxPenetrationDepth) {
    return character.SetShape(shape.GetPtr(), maxPenetrationDepth);
}

void Physics3DCharacter::addToSimulation(JPH::EActivation activation) {
    if (isAdded) { return; }
    character.AddToPhysicsSystem(activation);
    isAdded = true;
}

void Physics3DCharacter::removeFromSimulation() {
    if (!isAdded) { return; }
    character.RemoveFromPhysicsSystem();
    isAdded = false;
}

Physics3DCharacter::Physics3DCharacter(ecs::Entity entity, engine::World3D& world,
                                       Transform3D* transform, const JPH::CharacterSettings& s)
: settings(s)
, character(&settings, jvec(transform ? transform->getPosition() : glm::vec3()),
            jquat(transform ? transform->getRotation() : glm::quat(1.f, 0.f, 0.f, 0.f)), entity.id,
            &world.getPhysicsSystem())
, transform(transform)
, maxSeparationDistance(MaxSeparationDistance)
, isAdded(false) {
    if (!transform) {
        BL_LOG_WARN << "Physics3DCharacter component created for entity " << entity
                    << " that does not have a Transform3D component. Adding one.";
        transform = world.engine().ecs().emplaceComponent<Transform3D>(entity);
    }
}

void Physics3DCharacter::postUpdate() {
    if (isAdded) {
        character.PostSimulation(maxSeparationDistance, true);

        JPH::RVec3 pos;
        JPH::Quat rot;
        character.GetPositionAndRotation(pos, rot);
        transform->setPosition(glm::vec3(pos.GetX(), pos.GetY(), pos.GetZ()));
        transform->setRotation(glm::quat(rot.GetW(), rot.GetX(), rot.GetY(), rot.GetZ()));
    }
}

} // namespace com
} // namespace bl
