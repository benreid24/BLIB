#ifndef BLIB_PARTICLES_RENDERCONFIGMAP_HPP
#define BLIB_PARTICLES_RENDERCONFIGMAP_HPP

#include <BLIB/Render/Config.hpp>
#include <BLIB/Render/Vulkan/PipelineParameters.hpp>
#include <cstdint>
#include <initializer_list>
#include <limits>
#include <variant>

namespace bl
{
namespace pcl
{
namespace priv
{
constexpr std::uint32_t PipelineNotSet = std::numeric_limits<std::uint32_t>::max();
}

/**
 * @brief Helper struct that provides some sane defaults for the various render settings specified
 *        in RenderConfigMap
 *
 * @tparam T The particle type to generate defaults for
 * @ingroup Particles
 */
template<typename T>
struct RenderConfigDefaults {
    static constexpr std::initializer_list<std::uint32_t> RenderPassIds = {
        bl::rc::Config::RenderPassIds::StandardAttachmentDefault,
        bl::rc::Config::RenderPassIds::SwapchainDefault};
    using GlobalShaderPayload = std::monostate;
};

/**
 * @brief Helper struct to encode a variable list of descriptor sets for render pipeline creation
 *
 * @tparam ...TFactories The ordered list of descriptor sets to use to create the pipeline
 * @ingroup Particles
 */
template<typename... TFactories>
struct RenderConfigDescriptorList {
    static void addParameters(rc::vk::PipelineParameters& params) {
        (params.addDescriptorSet<TFactories>(), ...);
    }
};

/**
 * @brief Compile-time mapping struct for use with the default Renderer<T> implementation.
 *        Specialize this for your particle type and provide the pipeline and transparency settings
 *        to render with
 *
 * @tparam T The particle type to specify render settings for
 * @ingroup Particles
 */
template<typename T>
struct RenderConfigMap {
    /// The id of the pipeline to render the particle system with
    static constexpr std::uint32_t PipelineId = priv::PipelineNotSet;

    /// The transparency setting to render the particle system with
    static constexpr bool ContainsTransparency = false;

    /// Whether to create the GPU pipeline or not
    static constexpr bool CreateRenderPipeline = true;

    /// List of descriptor sets to create the pipeline with. Use RenderConfigDescriptorList to
    /// encode the list
    using DescriptorSets = RenderConfigDescriptorList<>;

    /// Whether to enable depth testing in the created pipeline
    static constexpr bool EnableDepthTesting = true;

    /// The vertex shader to create the pipeline with
    static constexpr const char* VertexShader = nullptr;

    /// The fragment shader to create the pipeline with
    static constexpr const char* FragmentShader = nullptr;

    /// The list of render pass ids the pipeline will be used with
    static constexpr std::initializer_list<std::uint32_t> RenderPassIds =
        RenderConfigDefaults<T>::RenderPassIds;

    /// The topology type the pipeline should be created with
    static constexpr VkPrimitiveTopology Topology = VK_PRIMITIVE_TOPOLOGY_POINT_LIST;

    /// Type of the global struct for the default descriptor set implementation
    using GlobalShaderPayload = RenderConfigDefaults<T>::GlobalShaderPayload;
};

} // namespace pcl
} // namespace bl

#endif
