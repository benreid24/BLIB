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
                                                 const com::Physics3D::BodySettings& bodySettings,
                                                 JPH::EActivation activation) {
    createPhysicsBody(
        createPhysicsShape(bodySettings.material), motionType, layer, bodySettings, activation);
}

void bl::obj::bcom::Physics3D::createPhysicsBody(JPH::Ref<JPH::Shape> shape,
                                                 JPH::EMotionType motionType,
                                                 com::Physics3D::ObjectLayer layer,
                                                 const com::Physics3D::BodySettings& bodySettings,
                                                 JPH::EActivation activation) {
    if (physics) {
        BL_LOG_WARN << "Physics body already created for entity " << entity()
                    << ". Ignoring request to create a new physics body";
        return;
    }

    physics = engine().ecs().emplaceComponent<com::Physics3D>(
        entity(), engine(), entity(), std::move(shape), motionType, layer, activation);
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
