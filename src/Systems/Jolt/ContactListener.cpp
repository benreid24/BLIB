#include <BLIB/Systems/Jolt/ContactListener.hpp>

#include <Jolt/Physics/Body/Body.h>

namespace bl
{
namespace sys
{
namespace jolt
{
ContactListener::ContactListener()
: bodyInterface(nullptr) {}

void ContactListener::init(sig::Channel& channel, JPH::BodyInterface& bodyInterface) {
    emitter.connect(channel);
    this->bodyInterface = &bodyInterface;
}

void ContactListener::OnContactAdded(const JPH::Body& body1, const JPH::Body& body2,
                                     const JPH::ContactManifold&, JPH::ContactSettings&) {
    const ecs::Entity entityA = body1.GetUserData();
    const ecs::Entity entityB = body2.GetUserData();
    emitter.emit(CollisionStartedEvent{entityA, body1, entityB, body2});
}

void ContactListener::OnContactPersisted(const JPH::Body& body1, const JPH::Body& body2,
                                         const JPH::ContactManifold&, JPH::ContactSettings&) {
    const ecs::Entity entityA = body1.GetUserData();
    const ecs::Entity entityB = body2.GetUserData();
    emitter.emit(CollisionPersistedEvent{entityA, body1, entityB, body2});
}

void ContactListener::OnContactRemoved(const JPH::SubShapeIDPair& shapePair) {
    const ecs::Entity entityA = bodyInterface->GetUserData(shapePair.GetBody1ID());
    const ecs::Entity entityB = bodyInterface->GetUserData(shapePair.GetBody2ID());
    emitter.emit(CollisionEndedEvent{entityA, entityB});
}

} // namespace jolt
} // namespace sys
} // namespace bl
