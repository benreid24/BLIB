#ifndef BLIB_RENDER_VULKAN_COMPUTEPIPELINE_HPP
#define BLIB_RENDER_VULKAN_COMPUTEPIPELINE_HPP

#include <BLIB/Render/Vulkan/ComputePipelineParameters.hpp>

namespace bl
{
namespace rc
{
class Renderer;

namespace vk
{
class PipelineLayout;

/**
 * @brief Instance of a compute pipeline
 *
 * @ingroup Renderer
 */
class ComputePipeline {
public:
    /**
     * @brief Creates a new compute pipeline
     *
     * @param renderer The renderer instance
     * @param id The id of the pipeline
     * @param params The parameters to create the pipeline with
     */
    ComputePipeline(Renderer& renderer, std::uint32_t id, ComputePipelineParameters&& params);

    /**
     * @brief Destroys the pipeline and frees Vulkan resources
     */
    ~ComputePipeline();

    /**
     * @brief Returns the layout of this pipeline
     */
    const PipelineLayout& pipelineLayout() const;

    /**
     * @brief Returns the underlying Vulkan pipeline handle
     */
    VkPipeline rawPipeline();

    /**
     * @brief Issues the command to bind the pipeline
     *
     * @param commandBuffer The command buffer to record into
     */
    void bind(VkCommandBuffer commandBuffer);

    /**
     * @brief Issues the command to dispatch the compute shader
     *
     * @param commandBuffer The command buffer to record into
     * @param groupCountX The number of work groups to dispatch in the X dimension
     * @param groupCountY The number of work groups to dispatch in the Y dimension
     * @param groupCountZ The number of work groups to dispatch in the Z dimension
     */
    void exec(VkCommandBuffer commandBuffer, std::uint32_t groupCountX,
              std::uint32_t groupCountY = 1, std::uint32_t groupCountZ = 1);

    /**
     * @brief Returns the id of this pipeline
     */
    std::uint32_t getId() const { return id; }

private:
    std::uint32_t id;
    Renderer& renderer;
    PipelineLayout* layout;
    VkPipeline pipeline;
    ComputePipelineParameters createParams;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

inline VkPipeline ComputePipeline::rawPipeline() { return pipeline; }

inline const PipelineLayout& ComputePipeline::pipelineLayout() const { return *layout; }

} // namespace vk
} // namespace rc
} // namespace bl

#endif
