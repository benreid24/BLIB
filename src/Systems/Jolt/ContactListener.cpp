#include <BLIB/Systems/Jolt/ContactListener.hpp>

#include <Jolt/Physics/Body/Body.h>

namespace bl
{
namespace sys
{
namespace jolt
{
ContactListener::ContactListener()
: bodyInterface(nullptr) {
    queuedContactRemovals.reserve(64);
}

void ContactListener::init(sig::Channel& channel, JPH::BodyInterface& bodyInterface) {
    emitter.connect(channel);
    this->bodyInterface = &bodyInterface;
}

void ContactListener::flushContactRemovals() {
    for (const auto& pair : queuedContactRemovals) {
        const ecs::Entity entityA = bodyInterface->GetUserData(pair.first);
        const ecs::Entity entityB = bodyInterface->GetUserData(pair.second);
        if (entityA.id != 0 && entityB.id != 0) {
            emitter.emit(CollisionEndedEvent{entityA, entityB});
        }
    }
    queuedContactRemovals.clear();
}

void ContactListener::OnContactAdded(const JPH::Body& body1, const JPH::Body& body2,
                                     const JPH::ContactManifold&, JPH::ContactSettings&) {
    const ecs::Entity entityA = body1.GetUserData();
    const ecs::Entity entityB = body2.GetUserData();
    emitter.emit(CollisionStartedEvent{entityA, entityB});
}

void ContactListener::OnContactPersisted(const JPH::Body& body1, const JPH::Body& body2,
                                         const JPH::ContactManifold&, JPH::ContactSettings&) {
    const ecs::Entity entityA = body1.GetUserData();
    const ecs::Entity entityB = body2.GetUserData();
    emitter.emit(CollisionPersistedEvent{entityA, entityB});
}

void ContactListener::OnContactRemoved(const JPH::SubShapeIDPair& shapePair) {
    queuedContactRemovals.emplace_back(shapePair.GetBody1ID(), shapePair.GetBody2ID());
}

} // namespace jolt
} // namespace sys
} // namespace bl
