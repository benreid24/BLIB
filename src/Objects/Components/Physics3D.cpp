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
                                                 JPH::EActivation activation, bool batch) {
    createPhysicsBody(createPhysicsShape(bodySettings.material),
                      motionType,
                      layer,
                      bodySettings,
                      activation,
                      batch);
}

void bl::obj::bcom::Physics3D::createPhysicsBody(JPH::Ref<JPH::Shape> shape,
                                                 JPH::EMotionType motionType,
                                                 com::Physics3D::ObjectLayer layer,
                                                 const com::Physics3D::BodySettings& bodySettings,
                                                 JPH::EActivation activation, bool batch) {
    if (physics) {
        BL_LOG_WARN << "Physics body already created for entity " << entity()
                    << ". Ignoring request to create a new physics body";
        return;
    }

    physics = engine().ecs().emplaceComponent<com::Physics3D>(entity(),
                                                              engine(),
                                                              entity(),
                                                              std::move(shape),
                                                              motionType,
                                                              layer,
                                                              activation,
                                                              bodySettings,
                                                              batch);
}

} // namespace bcom
} // namespace obj
} // namespace bl
