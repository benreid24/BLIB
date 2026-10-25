#ifndef BLIB_SYSTEMS_JOLT_BROADPHASELAYERINTERFACE_HPP
#define BLIB_SYSTEMS_JOLT_BROADPHASELAYERINTERFACE_HPP

#include <Jolt/Jolt.h>

#include <BLIB/Components/Physics3D.hpp>
#include <BLIB/Containers/StaticVector.hpp>
#include <Jolt/Physics/Collision/BroadPhase/BroadPhase.h>
#include <array>

namespace bl
{
namespace sys
{
namespace jolt
{
/**
 * @brief Mapping interface from object layers to broadphase layers
 *
 * @ingroup Systems
 */
class BroadPhaseLayerInterface : public JPH::BroadPhaseLayerInterface {
public:
    /**
     * @brief Creates the default mapping from com::Physics3D
     */
    BroadPhaseLayerInterface();

    /**
     * @brief Destroys the interface
     */
    virtual ~BroadPhaseLayerInterface() = default;

    /**
     * @brief Returns the number of broadphase layers
     */
    virtual std::uint32_t GetNumBroadPhaseLayers() const override;

    /**
     * @brief Returns the broadphase layer for the given object layer
     *
     * @param inLayer The object layer to get the broadphase layer for
     * @return The broadphase layer corresponding to the object layer
     */
    virtual JPH::BroadPhaseLayer GetBroadPhaseLayer(JPH::ObjectLayer inLayer) const override;

#if defined(JPH_EXTERNAL_PROFILE) || defined(JPH_PROFILE_ENABLED)
    /**
     * @brief Returns the name of the broadphase layer for debugging purposes
     *
     * @param inLayer The broadphase layer to get the name for
     * @return The name of the broadphase layer
     */
    virtual const char* GetBroadPhaseLayerName(JPH::BroadPhaseLayer inLayer) const override;
#endif

    /**
     * @brief Updates the mapping from object layer to broadphase layer
     *
     * @param objectLayer The object layer to set the mapping for
     * @param broadphase The broadphase layer to map the object layer to
     */
    void setObjectLayerBroadphase(com::Physics3D::ObjectLayer objectLayer,
                                  com::Physics3D::BroadphaseLayer broadphase);

    /**
     * @brief Adds or overwrites a new broadphase layer with the given name
     *
     * @param layer The new broadphase layer to add
     * @param name The name of the new broadphase layer for debugging purposes
     */
    void addBroadphaseLayer(com::Physics3D::BroadphaseLayer layer, const char* name);

private:
    std::uint32_t numLayers;
    std::array<JPH::BroadPhaseLayer, com::Physics3D::MaxObjectLayers> layerMap;

#if defined(JPH_EXTERNAL_PROFILE) || defined(JPH_PROFILE_ENABLED)
    ctr::StaticVector<const char*, com::Physics3D::MaxBroadphaseLayers> layerNames;
#endif
};

} // namespace jolt
} // namespace sys
} // namespace bl

#endif
