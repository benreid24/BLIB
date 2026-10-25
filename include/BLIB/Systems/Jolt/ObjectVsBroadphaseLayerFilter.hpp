#ifndef BLIB_SYSTEMS_JOLT_OBJECTVSBROADPHASELAYERFILTER_HPP
#define BLIB_SYSTEMS_JOLT_OBJECTVSBROADPHASELAYERFILTER_HPP

#include <Jolt/Jolt.h>

#include <BLIB/Components/Physics3D.hpp>
#include <Jolt/Physics/Collision/BroadPhase/BroadPhaseLayer.h>
#include <array>

namespace bl
{
namespace sys
{
namespace jolt
{
class ObjectLayerPairFilter;
class BroadPhaseLayerInterface;

/**
 * @brief Collision filter for object layers vs broadphase layers
 *
 * @ingroup Systems
 */
class ObjectVsBroadPhaseLayerFilter : public JPH::ObjectVsBroadPhaseLayerFilter {
public:
    /**
     * @brief Creates the default filter from com::Physics3D
     */
    ObjectVsBroadPhaseLayerFilter();

    /**
     * @brief Destroys the filter
     */
    virtual ~ObjectVsBroadPhaseLayerFilter() = default;

    /**
     * @brief Tests whether an object layer collides with a broadphase layer
     *
     * @param inLayer1 The object layer
     * @param inLayer2 The broadphase layer
     * @return True if the two layers should collide, false otherwise
     */
    virtual bool ShouldCollide(JPH::ObjectLayer inLayer1,
                               JPH::BroadPhaseLayer inLayer2) const override;

    /**
     * @brief Updates the mapping based on object layer collision and broadphase layer mapping
     *
     * @param objectPairLayerFilter The object layer pair filter to use for collision testing
     * @param broadphaseLayerInterface The broadphase layer mapping
     */
    void update(const ObjectLayerPairFilter& objectPairLayerFilter,
                const BroadPhaseLayerInterface& broadphaseLayerInterface);

private:
    std::array<JPH::BroadPhaseLayer::Type, com::Physics3D::MaxObjectLayers> objectLayerMasks;
};

} // namespace jolt
} // namespace sys
} // namespace bl

#endif
