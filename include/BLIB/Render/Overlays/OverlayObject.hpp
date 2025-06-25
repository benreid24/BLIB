#ifndef BLIB_RENDER_OVERLAYS_OVERLAYOBJECT_HPP
#define BLIB_RENDER_OVERLAYS_OVERLAYOBJECT_HPP

#include <BLIB/Components/OverlayScaler.hpp>
#include <BLIB/ECS.hpp>
#include <BLIB/ECS/Traits/ParentAware.hpp>
#include <BLIB/Render/Config/Limits.hpp>
#include <BLIB/Render/Descriptors/DescriptorSetInstance.hpp>
#include <BLIB/Render/Materials/MaterialPipeline.hpp>
#include <BLIB/Render/Primitives/DrawParameters.hpp>
#include <BLIB/Render/Scenes/Key.hpp>
#include <BLIB/Render/Scenes/SceneObject.hpp>
#include <BLIB/Vulkan.hpp>
#include <array>
#include <cstdint>
#include <vector>

namespace bl
{
namespace rc
{
class Overlay;

/// Classes for Observer overlays
namespace ovy
{
struct OverlayObject
: public scene::SceneObject
, public ecs::trait::ParentAware<OverlayObject>
, public ecs::trait::ChildAware<OverlayObject> {
    /**
     * @brief Construct a new OverlayObject
     */
    OverlayObject();

    ecs::Entity entity;
    Overlay* overlay;
    mat::MaterialPipeline* pipeline;
    std::array<ds::DescriptorSetInstance*, cfg::Limits::MaxDescriptorSets> descriptors;
    std::uint8_t descriptorCount;
    std::uint8_t perObjStart;

    VkViewport* overlayViewport;
    VkRect2D cachedScissor;
};

} // namespace ovy
} // namespace rc
} // namespace bl

#endif
