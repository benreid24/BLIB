#include <BLIB/Objects/Components/Physics3D.hpp>

#include <BLIB/Engine/Engine.hpp>

namespace bl
{
namespace obj
{
namespace bcom
{
Physics3D::Physics3D()
: physics(nullptr) {}

void bl::obj::bcom::Physics3D::createPhysicsBody(JPH::EMotionType motionType,
                                                 com::Physics3D::ObjectLayer layer,
                                                 JPH::EActivation activation) {
    engine::Engine* engine = nullptr;
    ecs::Entity entity;
    getEngineAndEntity(engine, entity);

    if (physics) {
        BL_LOG_WARN << "Physics body already created for entity " << entity
                    << ". Ignoring request to create a new physics body";
        return;
    }

    if (engine) {
        physics = engine->ecs().emplaceComponent<com::Physics3D>(
            entity, *engine, entity, createPhysicsShape(), motionType, layer, activation);
    }
    else {
        BL_LOG_ERROR << "Failed to create physics body for entity " << entity
                     << " because the engine instance could not be retrieved";
    }
}

bool Physics3D::isAddedToPhysicsSimulation() const {
    return physics ? physics->isAddedToSimulation() : false;
}

void Physics3D::addToPhysicsSimulation(JPH::EActivation activation) {
    if (physics) { physics->addToSimulation(activation); }
}

void Physics3D::removeFromPhysicsSimulation() {
    if (physics) { physics->removeFromSimulation(); }
}

void Physics3D::setLinearVelocity(const glm::vec3& velocity) {
    if (physics) { physics->setLinearVelocity(velocity); }
}

void Physics3D::setPosition(const glm::vec3& position, JPH::EActivation activation) {
    if (physics) { physics->setPosition(position, activation); }
}

} // namespace bcom
} // namespace obj
} // namespace bl
