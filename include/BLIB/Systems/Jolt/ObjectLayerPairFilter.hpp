#ifndef BLIB_SYSTEMS_JOLT_OBJECTLAYERPAIRFILTER_HPP
#define BLIB_SYSTEMS_JOLT_OBJECTLAYERPAIRFILTER_HPP

#include <Jolt/Jolt.h>

#include <BLIB/Components/Physics3D.hpp>
#include <Jolt/Physics/Collision/ObjectLayer.h>
#include <limits>

namespace bl
{
namespace sys
{
namespace jolt
{
/**
 * @brief Object layer collision filter implementation built on bitmasking
 *
 * @ingroup Systems
 */
class ObjectLayerPairFilter : public JPH::ObjectLayerPairFilter {
public:
    static constexpr std::size_t MaxObjectLayers = std::numeric_limits<JPH::ObjectLayer>::digits;

    /**
     * @brief Creates the default filters from com::Physics3D::ObjectCollisionMasks
     */
    ObjectLayerPairFilter();

    /**
     * @brief Destroys the filter
     */
    virtual ~ObjectLayerPairFilter() = default;

    /**
     * @brief Tests whether two object layers should collide based on the bitmasking rules
     *
     * @param inLayer1 The first object layer
     * @param inLayer2 The second object layer
     * @return True if the two layers should collide, false otherwise
     */
    virtual bool ShouldCollide(JPH::ObjectLayer inLayer1, JPH::ObjectLayer inLayer2) const override;

    /**
     * @brief Overrides the collision mask for the given object layer
     *
     * @param layer The layer to set the mask for
     * @param mask The mask to use for the layer
     */
    void setObjectLayerCollisionMask(com::Physics3D::ObjectLayer layer, std::uint16_t mask);

private:
    JPH::ObjectLayer layerMasks[MaxObjectLayers];
};

} // namespace jolt
} // namespace sys
} // namespace bl

#endif
