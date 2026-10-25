#include <BLIB/Systems/Jolt/ObjectVsBroadphaseLayerFilter.hpp>

#include <BLIB/Systems/Jolt/BroadphaseLayerInterface.hpp>
#include <BLIB/Systems/Jolt/ObjectLayerPairFilter.hpp>

namespace bl
{
namespace sys
{
namespace jolt
{
using T = JPH::BroadPhaseLayer::Type;

ObjectVsBroadPhaseLayerFilter::ObjectVsBroadPhaseLayerFilter()
: objectLayerMasks{} {}

bool ObjectVsBroadPhaseLayerFilter::ShouldCollide(JPH::ObjectLayer inLayer1,
                                                  JPH::BroadPhaseLayer inLayer2) const {
    const T mask      = objectLayerMasks[inLayer1];
    const T testLayer = inLayer2.GetValue();
    return (mask & (1 << testLayer)) != 0;
}

void ObjectVsBroadPhaseLayerFilter::update(
    const ObjectLayerPairFilter& objectPairLayerFilter,
    const BroadPhaseLayerInterface& broadphaseLayerInterface) {
    objectLayerMasks.fill(0);

    for (unsigned int i = 0; i < com::Physics3D::MaxObjectLayers; ++i) {
        for (unsigned int j = 0; j < com::Physics3D::MaxObjectLayers; ++j) {
            if (objectPairLayerFilter.ShouldCollide(static_cast<JPH::ObjectLayer>(i),
                                                    static_cast<JPH::ObjectLayer>(j))) {
                objectLayerMasks[i] |=
                    (1 << broadphaseLayerInterface
                              .GetBroadPhaseLayer(static_cast<JPH::ObjectLayer>(j))
                              .GetValue());
            }
        }
    }
}

} // namespace jolt
} // namespace sys
} // namespace bl
