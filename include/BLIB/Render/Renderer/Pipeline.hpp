#ifndef BLIB_RENDER_RENDERER_PIPELINE_HP
#define BLIB_RENDER_RENDERER_PIPELINE_HP

#include <BLIB/Render/Renderer/PipelineParameters.hpp>
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

private:
    Renderer& renderer;
    VkPipelineLayout layout;
    VkPipeline pipeline;
    std::vector<PipelineParameters::DescriptorSetRetriever> descriptorGetters;
    std::vector<VkDescriptorSet> descriptorSets;

    Pipeline(Renderer& renderer, PipelineParameters&& params);

    friend class PipelineCache;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

inline constexpr VkPipelineLayout Pipeline::pipelineLayout() const { return layout; }

} // namespace render
} // namespace bl

#endif
