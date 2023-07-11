#ifndef BLIB_RENDER_VULKAN_PIPELINE_HPP
#define BLIB_RENDER_VULKAN_PIPELINE_HPP

#include <BLIB/Render/Config.hpp>
#include <BLIB/Render/Descriptors/DescriptorSetInstanceCache.hpp>
#include <BLIB/Render/Vulkan/PipelineLayout.hpp>
#include <BLIB/Render/Vulkan/PipelineParameters.hpp>
#include <BLIB/Render/Vulkan/VulkanState.hpp>
#include <BLIB/Vulkan.hpp>
#include <array>
#include <string>
#include <variant>
#include <vector>

namespace bl
{
namespace gfx
{
class Renderer;
class PipelineCache;

namespace vk
{
/**
 * @brief Core renderer class representing a pipeline that is a part of a RenderPass. Pipelines are
 *        managed by the Renderer's PipelineCache and should not be created directly
 *
 * @ingroup Renderer
 */
class Pipeline {
public:
    /**
     * @brief Construct a new Pipeline
     *
     * @param renderer The renderer the pipeline will belong to
     * @param params The parameters to create the pipeline with
     */
    Pipeline(Renderer& renderer, PipelineParameters&& params);

    /**
     * @brief Destroy the Pipeline and frees Vulkan resources
     *
     */
    ~Pipeline();

    /**
     * @brief Returns the layout of this pipeline
     *
     * @return constexpr VkPipelineLayout The layout of this pipeline
     */
    constexpr const PipelineLayout& pipelineLayout() const;

    /**
     * @brief Returns the underlying Vulkan pipeline handle
     *
     * @param renderPassId The render pass to use to get the specific pipeline
     */
    VkPipeline rawPipeline(std::uint32_t renderPassId) const;

    /**
     * @brief Issues the command to bind the pipeline
     *
     * @param commandBuffer The command buffer to record into
     * @param renderPassId The current render pass
     */
    void bind(VkCommandBuffer commandBuffer, std::uint32_t renderPassId);

private:
    Renderer& renderer;
    PipelineLayout* layout;
    std::array<VkPipeline, Config::MaxRenderPasses> pipelines;

    friend class PipelineCache;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

inline constexpr const PipelineLayout& Pipeline::pipelineLayout() const { return *layout; }

inline VkPipeline Pipeline::rawPipeline(std::uint32_t rpid) const { return pipelines[rpid]; }

} // namespace vk
} // namespace gfx
} // namespace bl

#endif
