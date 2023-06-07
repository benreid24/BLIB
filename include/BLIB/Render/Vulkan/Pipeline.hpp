#ifndef BLIB_RENDER_VULKAN_PIPELINE_HPP
#define BLIB_RENDER_VULKAN_PIPELINE_HPP

#include <BLIB/Render/Config.hpp>
#include <BLIB/Render/Descriptors/DescriptorSetInstanceCache.hpp>
#include <BLIB/Render/Vulkan/PipelineParameters.hpp>
#include <BLIB/Render/Vulkan/VulkanState.hpp>
#include <array>
#include <glad/vulkan.h>
#include <string>
#include <variant>
#include <vector>

namespace bl
{
namespace render
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
    constexpr VkPipelineLayout pipelineLayout() const;

    /**
     * @brief Returns whether or not batched objects in this pipeline should have their order
     *        preserved as objects are added and removed
     */
    constexpr bool preserveObjectOrder() const;

    /**
     * @brief Returns the underlying Vulkan pipeline handle
     *
     * @param renderPassId The render pass to use to get the specific pipeline
     */
    VkPipeline rawPipeline(std::uint32_t renderPassId) const;

    /**
     * @brief Creates descriptor set instances for this pipeline
     *
     * @param cache Descriptor set cache to use when creating or fetching sets
     * @param descriptors Vector of descriptor sets to populate
     */
    void createDescriptorSets(ds::DescriptorSetInstanceCache& cache,
                              std::vector<ds::DescriptorSetInstance*>& descriptors);

    /**
     * @brief Initializes the given array of descriptor sets and returns the number of sets
     *
     * @param cache Descriptor set cache to use when creating or fetching sets
     * @param sets Pointer to an array of descriptor set pointers
     * @return The number of descriptor sets used by this pipeline
     */
    std::uint32_t initDescriptorSets(ds::DescriptorSetInstanceCache& cache,
                                     ds::DescriptorSetInstance** sets);

    /**
     * @brief Issues the command to bind the pipeline
     *
     * @param commandBuffer The command buffer to record into
     * @param renderPassId The current render pass
     */
    void bind(VkCommandBuffer commandBuffer, std::uint32_t renderPassId);

private:
    Renderer& renderer;
    VkPipelineLayout layout;
    std::array<VkPipeline, Config::MaxRenderPasses> pipelines;
    std::vector<ds::DescriptorSetFactory*> descriptorSets;
    bool preserveOrder;

    friend class PipelineCache;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

inline constexpr VkPipelineLayout Pipeline::pipelineLayout() const { return layout; }

inline constexpr bool Pipeline::preserveObjectOrder() const { return preserveOrder; }

inline VkPipeline Pipeline::rawPipeline(std::uint32_t rpid) const { return pipelines[rpid]; }

} // namespace vk
} // namespace render
} // namespace bl

#endif
