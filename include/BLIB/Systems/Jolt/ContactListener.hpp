#ifndef BLIB_SYSTEMS_JOLT_CONTACTLISTENER_HPP
#define BLIB_SYSTEMS_JOLT_CONTACTLISTENER_HPP

#include <Jolt/Jolt.h>

#include <BLIB/Signals/Emitter.hpp>
#include <BLIB/Systems/Jolt/Signals.hpp>
#include <Jolt/Physics/Body/BodyInterface.h>
#include <Jolt/Physics/Collision/ContactListener.h>
#include <vector>

namespace bl
{
namespace sys
{
namespace jolt
{
/**
 * @brief Event interface to map jolt contact events to engine collision events
 *
 * @ingroup Systems
 */
class ContactListener : public JPH::ContactListener {
public:
    /**
     * @brief Initializes the contact listener
     */
    ContactListener();

    /**
     * @brief Sets up the contact listener for use
     *
     * @param channel The channel to emit events on
     * @param bodyInterface The interface to use to interact with Jolt
     */
    void init(sig::Channel& channel, JPH::BodyInterface& bodyInterface);

    /**
     * @brief Destroys the contact listener
     */
    virtual ~ContactListener() = default;

private:
    sig::Emitter<CollisionStartedEvent, CollisionPersistedEvent, CollisionEndedEvent> emitter;
    JPH::BodyInterface* bodyInterface;

    virtual void OnContactAdded(const JPH::Body& body1, const JPH::Body& body2,
                                const JPH::ContactManifold& manifold,
                                JPH::ContactSettings& settings) override;

    virtual void OnContactPersisted(const JPH::Body& body1, const JPH::Body& body2,
                                    const JPH::ContactManifold& manifold,
                                    JPH::ContactSettings& settings) override;

    virtual void OnContactRemoved(const JPH::SubShapeIDPair& shapePair) override;
};

} // namespace jolt
} // namespace sys
} // namespace bl

#endif
