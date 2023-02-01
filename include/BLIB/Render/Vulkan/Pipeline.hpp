#ifndef BLIB_RENDER_VULKAN_PIPELINE_HPP
#define BLIB_RENDER_VULKAN_PIPELINE_HPP

#include <BLIB/Render/Vulkan/PipelineParameters.hpp>
#include <BLIB/Render/Vulkan/VulkanState.hpp>
#include <glad/vulkan.h>
#include <string>
#include <vector>

namespace bl
{
namespace render
{
class Renderer;
class PipelineCache;

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
     * @brief Binds the pipeline and its descriptor sets
     *
     * @param commandBuffer The command buffer to perform the bind in
     */
    void bindPipelineAndDescriptors(VkCommandBuffer commandBuffer);

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
     */
    constexpr VkPipeline rawPipeline() const;

private:
    Renderer& renderer;
    VkPipelineLayout layout;
    VkPipeline pipeline;
    std::vector<PipelineParameters::DescriptorSetRetriever> descriptorGetters;
    std::vector<VkDescriptorSet> descriptorSets;
    bool preserveOrder;

    friend class PipelineCache;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

inline constexpr VkPipelineLayout Pipeline::pipelineLayout() const { return layout; }

inline constexpr bool Pipeline::preserveObjectOrder() const { return preserveOrder; }

inline constexpr VkPipeline Pipeline::rawPipeline() const { return pipeline; }

} // namespace render
} // namespace bl

#endif
