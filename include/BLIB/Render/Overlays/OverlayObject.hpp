#ifndef BLIB_RENDER_OVERLAYS_OVERLAYOBJECT_HPP
#define BLIB_RENDER_OVERLAYS_OVERLAYOBJECT_HPP

#include <BLIB/Render/Config.hpp>
#include <BLIB/Render/Descriptors/DescriptorSetInstance.hpp>
#include <BLIB/Render/Overlays/Viewport.hpp>
#include <BLIB/Render/Primitives/DrawParameters.hpp>
#include <BLIB/Render/Scenes/SceneObject.hpp>
#include <BLIB/Render/Vulkan/Pipeline.hpp>
#include <array>
#include <cstdint>
#include <glad/vulkan.h>
#include <optional>
#include <vector>

namespace bl
{
namespace render
{
/// Classes for Observer overlays
namespace ovy
{
struct OverlayObject : public scene::SceneObject {
    /**
     * @brief Construct a new OverlayObject
     */
    OverlayObject();

    /**
     * @brief Adds the child to this object's child array
     *
     * @param childId The id of the child to add
     */
    void registerChild(std::uint32_t childId);

    /**
     * @brief Removes the given child from this object's child array
     *
     * @param childId The child id to remove
     */
    void removeChild(std::uint32_t childId);

    std::vector<std::uint32_t> children;
    std::optional<Viewport> viewport;
    vk::Pipeline* pipeline;
    std::array<ds::DescriptorSetInstance*, Config::MaxDescriptorSets> descriptors;
    std::uint8_t descriptorCount;
};

} // namespace ovy
} // namespace render
} // namespace bl

#endif
