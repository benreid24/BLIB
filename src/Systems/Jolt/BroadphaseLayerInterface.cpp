#include <BLIB/Systems/Jolt/BroadphaseLayerInterface.hpp>

namespace bl
{
namespace sys
{
namespace jolt
{
namespace
{
constexpr JPH::BroadPhaseLayer DefaultLayer(
    static_cast<JPH::BroadPhaseLayer::Type>(com::Physics3D::BroadphaseLayer::Moving));
const char* MissingLayerName = "LayerNotNamed";
} // namespace

BroadPhaseLayerInterface::BroadPhaseLayerInterface()
: numLayers(static_cast<std::uint32_t>(com::Physics3D::BroadphaseLayer::Count)) {
    layerMap.fill(JPH::BroadPhaseLayer(
        static_cast<JPH::BroadPhaseLayer::Type>(com::Physics3D::BroadphaseLayer::Moving)));

    constexpr unsigned int ObjectLayerCount =
        std::size(com::Physics3D::BroadphaseObjectMapping::Mapping);
    for (unsigned int i = 0; i < ObjectLayerCount; ++i) {
        layerMap[i] = JPH::BroadPhaseLayer(static_cast<JPH::BroadPhaseLayer::Type>(
            com::Physics3D::BroadphaseObjectMapping::Mapping[i]));
    }

    addBroadphaseLayer(com::Physics3D::BroadphaseLayer::Moving, "Moving");
    addBroadphaseLayer(com::Physics3D::BroadphaseLayer::NonMoving, "NonMoving");
}

std::uint32_t BroadPhaseLayerInterface::GetNumBroadPhaseLayers() const { return numLayers; }

JPH::BroadPhaseLayer BroadPhaseLayerInterface::GetBroadPhaseLayer(JPH::ObjectLayer inLayer) const {
    return inLayer < layerMap.size() ? layerMap[inLayer] : DefaultLayer;
}

#if defined(JPH_EXTERNAL_PROFILE) || defined(JPH_PROFILE_ENABLED)
const char* BroadPhaseLayerInterface::GetBroadPhaseLayerName(JPH::BroadPhaseLayer inLayer) const {
    return inLayer < layerNames.size() ? layerNames[inLayer] : MissingLayerName;
}
#endif

void BroadPhaseLayerInterface::setObjectLayerBroadphase(
    com::Physics3D::ObjectLayer objectLayer, com::Physics3D::BroadphaseLayer broadphase) {
    layerMap[static_cast<std::size_t>(objectLayer)] =
        JPH::BroadPhaseLayer(static_cast<JPH::BroadPhaseLayer::Type>(broadphase));
}

void BroadPhaseLayerInterface::addBroadphaseLayer(com::Physics3D::BroadphaseLayer layer,
                                                  const char* name) {
    numLayers = std::max(numLayers, static_cast<std::uint32_t>(layer) + 1);

#if defined(JPH_EXTERNAL_PROFILE) || defined(JPH_PROFILE_ENABLED)
    if (layerNames.size() <= static_cast<std::size_t>(layer)) {
        layerNames.resize(static_cast<std::size_t>(layer) + 1, MissingLayerName);
    }
    layerNames[static_cast<std::size_t>(layer)] = name;
#else
    (void)layer;
    (void)name;
#endif
}

} // namespace jolt
} // namespace sys
} // namespace bl
