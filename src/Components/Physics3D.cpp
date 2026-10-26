#include <BLIB/Components/Physics3D.hpp>

#include <BLIB/Engine/Engine.hpp>
#include <BLIB/Engine/Worlds/World3D.hpp>
#include <BLIB/Logging.hpp>

namespace bl
{
namespace com
{
namespace
{
JPH::RVec3 jvec(const glm::vec3& v) { return JPH::RVec3(v.x, v.y, v.z); }

JPH::Quat jquat(const glm::quat& q) { return JPH::Quat(q.x, q.y, q.z, q.w); }
} // namespace

Physics3D::Physics3D(engine::Engine& engine, ecs::Entity entity,
                     std::unique_ptr<JPH::Shape>&& shape, JPH::EMotionType motionType,
                     ObjectLayer layer, JPH::EActivation activation)
: Physics3D(engine, entity, std::move(shape), motionType, layer) {
    addToSimulation(activation);
}

Physics3D::Physics3D(engine::Engine& engine, ecs::Entity entity,
                     std::unique_ptr<JPH::Shape>&& shape, JPH::EMotionType motionType,
                     ObjectLayer layer)
: entity(entity)
, bodyInterface(nullptr)
, shape(std::move(shape))
, transform(engine.ecs().getComponent<com::Transform3D>(entity))
, body(nullptr)
, isAdded(false) {
    if (!transform) {
        BL_LOG_WARN << "Physics3D component created for entity " << entity
                    << " that does not have a Transform3D component. Adding one.";
        transform = engine.ecs().emplaceComponent<com::Transform3D>(entity);
    }

    auto world               = engine.getWorld(entity.getWorldIndex());
    engine::World3D* world3d = dynamic_cast<engine::World3D*>(world.get());
    if (world3d) {
        bodyInterface = &world3d->getPhysicsSystem().GetBodyInterface();
        JPH::BodyCreationSettings settings(this->shape.get(),
                                           jvec(transform->getPosition()),
                                           jquat(transform->getRotation()),
                                           motionType,
                                           static_cast<JPH::ObjectLayer>(layer));
        body = bodyInterface->CreateBody(settings);
        if (body == nullptr) {
            BL_LOG_ERROR << "Failed to create physics body for entity" << entity;
        }
    }
    else {
        BL_LOG_ERROR << "Physics3D component created for entity (" << entity
                     << ") in world that is not a 3D world";
    }
}

void Physics3D::addToSimulation(JPH::EActivation activation) {
    if (isAdded) { return; }
    if (!bodyInterface || !body) {
        BL_LOG_WARN << "Cannot add entity " << entity << " to simulation as body creation failed";
        return;
    }

    bodyInterface->AddBody(body->GetID(), activation);
    isAdded = true;
}

void Physics3D::removeFromSimulation() {
    if (!isAdded) { return; }
    if (!bodyInterface || !body) {
        BL_LOG_WARN << "Cannot remove entity " << entity
                    << " from simulation as body creation failed";
        return;
    }

    bodyInterface->RemoveBody(body->GetID());
    isAdded = false;
}

void Physics3D::setLinearVelocity(const glm::vec3& velocity) {
    if (!isAdded) { return; }
    if (!bodyInterface || !body) {
        BL_LOG_WARN << "Cannot set velocity on entity " << entity << " as body creation failed";
        return;
    }

    bodyInterface->SetLinearVelocity(body->GetID(), jvec(velocity));
}

void Physics3D::setPosition(const glm::vec3& position, JPH::EActivation activation) {
    if (!isAdded) { return; }
    if (!bodyInterface || !body) {
        BL_LOG_WARN << "Cannot set position on entity " << entity << " as body creation failed";
        return;
    }

    bodyInterface->SetPosition(body->GetID(), jvec(position), activation);
}

void Physics3D::writebackTransform() {
    if (!isAdded || !body) { return; }

    JPH::RVec3 pos;
    JPH::Quat rot;
    bodyInterface->GetPositionAndRotation(body->GetID(), pos, rot);
    transform->setPosition(glm::vec3(pos.GetX(), pos.GetY(), pos.GetZ()));
    transform->setRotation(glm::quat(rot.GetW(), rot.GetX(), rot.GetY(), rot.GetZ()));
}

} // namespace com
} // namespace bl