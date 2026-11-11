#include <BLIB/Components/Physics3D.hpp>

#include <BLIB/Engine/Engine.hpp>
#include <BLIB/Engine/Worlds/World3D.hpp>
#include <BLIB/Logging.hpp>
#include <BLIB/Systems/Physics3D.hpp>
#include <Jolt/Physics/Body/BodyCreationSettings.h>

namespace bl
{
namespace com
{
namespace
{
JPH::RVec3 jvec(const glm::vec3& v) { return JPH::RVec3(v.x, v.y, v.z); }

JPH::Quat jquat(const glm::quat& q) { return JPH::Quat(q.x, q.y, q.z, q.w); }
} // namespace

Physics3D::Physics3D(engine::Engine& engine, ecs::Entity entity, JPH::Ref<JPH::Shape> shape,
                     JPH::EMotionType motionType, ObjectLayer layer, JPH::EActivation activation,
                     const BodySettings& bodySettings, bool batch)
: Physics3D(engine, entity, std::move(shape), motionType, layer, bodySettings) {
    addToSimulation(activation, batch);
}

Physics3D::Physics3D(engine::Engine& engine, ecs::Entity entity, JPH::Ref<JPH::Shape> shape,
                     JPH::EMotionType motionType, ObjectLayer layer,
                     const BodySettings& bodySettings)
: entity(entity)
, world(nullptr)
, shape(std::move(shape))
, transform(engine.ecs().getComponent<com::Transform3D>(entity))
, body(nullptr)
, isAdded(false) {
    if (!transform) {
        BL_LOG_WARN << "Physics3D component created for entity " << entity
                    << " that does not have a Transform3D component. Adding one.";
        transform = engine.ecs().emplaceComponent<com::Transform3D>(entity);
    }

    auto w = engine.getWorld(entity.getWorldIndex());
    world  = dynamic_cast<engine::World3D*>(w.get());
    if (world) {
        JPH::BodyCreationSettings settings(this->shape,
                                           jvec(transform->getPosition()),
                                           jquat(transform->getRotation()),
                                           motionType,
                                           static_cast<JPH::ObjectLayer>(layer));
        settings.mUserData           = entity.id;
        settings.mAllowedDOFs        = bodySettings.allowedDOFs;
        settings.mIsSensor           = bodySettings.isSensor;
        settings.mMotionQuality      = bodySettings.motionQuality;
        settings.mAllowSleeping      = bodySettings.allowSleeping;
        settings.mFriction           = bodySettings.friction;
        settings.mRestitution        = bodySettings.restitution;
        settings.mLinearDamping      = bodySettings.linearDamping;
        settings.mMaxLinearVelocity  = bodySettings.maxLinearVelocity;
        settings.mAngularDamping     = bodySettings.angularDamping;
        settings.mMaxAngularVelocity = bodySettings.maxAngularVelocity;
        settings.mGravityFactor      = bodySettings.gravityFactor;

        body = world->getPhysicsSystem().GetBodyInterface().CreateBody(settings);
        if (body == nullptr) {
            BL_LOG_ERROR << "Failed to create physics body for entity" << entity;
        }
    }
    else {
        BL_LOG_ERROR << "Physics3D component created for entity (" << entity
                     << ") in world that is not a 3D world";
    }
}

Physics3D::~Physics3D() {
    if (world && body) {
        if (isAdded) { world->queueRemovePhysicsBody(body->GetID()); }
        world->queueDestroyPhysicsBody(body->GetID());
    }
}

void Physics3D::addToSimulation(JPH::EActivation activation, bool batch) {
    if (isAdded) { return; }
    if (!world || !body) {
        BL_LOG_WARN << "Cannot add entity " << entity << " to simulation as body creation failed";
        return;
    }

    isAdded = true;
    if (!batch) { world->getPhysicsSystem().GetBodyInterface().AddBody(body->GetID(), activation); }
    else { world->queueAddPhysicsBody(body->GetID(), activation); }
}

void Physics3D::removeFromSimulation(bool batch) {
    if (!isAdded) { return; }
    if (!world || !body) {
        BL_LOG_WARN << "Cannot remove entity " << entity
                    << " from simulation as body creation failed";
        return;
    }

    isAdded = false;
    if (!batch) { world->getPhysicsSystem().GetBodyInterface().RemoveBody(body->GetID()); }
    else { world->queueRemovePhysicsBody(body->GetID()); }
}

void Physics3D::setLinearVelocity(const glm::vec3& velocity) {
    if (!isAdded) { return; }
    if (!world || !body) {
        BL_LOG_WARN << "Cannot set velocity on entity " << entity << " as body creation failed";
        return;
    }

    world->getPhysicsSystem().GetBodyInterface().SetLinearVelocity(body->GetID(), jvec(velocity));
}

void Physics3D::setPosition(const glm::vec3& position, JPH::EActivation activation) {
    if (!isAdded) { return; }
    if (!world || !body) {
        BL_LOG_WARN << "Cannot set position on entity " << entity << " as body creation failed";
        return;
    }

    world->getPhysicsSystem().GetBodyInterface().SetPosition(
        body->GetID(), jvec(position), activation);
}

void Physics3D::writebackTransform() {
    if (!isAdded || !body) { return; }

    JPH::RVec3 pos;
    JPH::Quat rot;
    world->getPhysicsSystem().GetBodyInterface().GetPositionAndRotation(body->GetID(), pos, rot);
    transform->setPosition(glm::vec3(pos.GetX(), pos.GetY(), pos.GetZ()));
    transform->setRotation(glm::quat(rot.GetW(), rot.GetX(), rot.GetY(), rot.GetZ()));
}

} // namespace com
} // namespace bl