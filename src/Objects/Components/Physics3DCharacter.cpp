#include <BLIB/Objects/Components/Physics3DCharacter.hpp>

#include <BLIB/Engine/Engine.hpp>
#include <BLIB/Engine/Worlds/World3D.hpp>

namespace bl
{
namespace obj
{
namespace bcom
{
Physics3DCharacter::Physics3DCharacter()
: character(nullptr) {}

void Physics3DCharacter::createPhysicsCharacter(const JPH::CharacterSettings& settings, bool add,
                                                JPH::Ref<JPH::PhysicsMaterial> material) {
    createPhysicsCharacter(settings, createPhysicsShape(material), add);
}

void Physics3DCharacter::createPhysicsCharacter(const JPH::CharacterSettings& settings,
                                                JPH::Ref<JPH::Shape> shape, bool add) {
    if (character) {
        BL_LOG_WARN << "Physics character already created for entity " << entity()
                    << ". Ignoring request to create a new physics character";
        return;
    }

    engine::World* world     = engine().getWorld(entity().getWorldIndex()).get();
    engine::World3D* world3d = dynamic_cast<engine::World3D*>(world);
    if (!world3d) {
        BL_LOG_ERROR << "Failed to create physics character for entity " << entity()
                     << " because the world is not a 3D world";
        return;
    }

    character = engine().ecs().emplaceComponent<com::Physics3DCharacter>(
        entity(), entity(), *world3d, settings);
    character->setShape(shape);
    if (add) { character->addToSimulation(); }
}

bool Physics3DCharacter::hasPhysicsCharacter() const { return character != nullptr; }

com::Physics3DCharacter* Physics3DCharacter::getPhysicsCharacter() {
    if (!character) {
        BL_LOG_ERROR << "Attempted to get physics character for entity " << entity()
                     << " but it has not been created";
    }
    return character;
}

} // namespace bcom
} // namespace obj
} // namespace bl
