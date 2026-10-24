#include <BLIB/Systems/Jolt/ObjectLayerPairFilter.hpp>

#include <BLIB/Components/Physics3D.hpp>

namespace bl
{
namespace sys
{
namespace jolt
{
namespace
{
constexpr unsigned int DefaultLayerCount =
    static_cast<unsigned int>(com::Physics3D::ObjectLayer::Count);
}

ObjectLayerPairFilter::ObjectLayerPairFilter()
: layerMasks{} {
    for (std::size_t i = 0; i < DefaultLayerCount; ++i) {
        layerMasks[i] = com::Physics3D::ObjectCollisionMasks::Masks[i];
    }
}

bool ObjectLayerPairFilter::ShouldCollide(JPH::ObjectLayer inLayer1,
                                          JPH::ObjectLayer inLayer2) const {
    const std::uint16_t mask1 = layerMasks[inLayer1];
    const std::uint16_t mask2 = layerMasks[inLayer2];
    return (mask1 & (1 << inLayer2)) != 0 && (mask2 & (1 << inLayer1)) != 0;
}

void ObjectLayerPairFilter::setObjectLayerCollisionMask(com::Physics3D::ObjectLayer layer,
                                                        std::uint16_t mask) {
    layerMasks[static_cast<std::size_t>(layer)] = mask;
}

} // namespace jolt
} // namespace sys
} // namespace bl
