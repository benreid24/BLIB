#ifndef BLIB_RENDER_VULKAN_PIPELINE_HPP
#define BLIB_RENDER_VULKAN_PIPELINE_HPP

#include <BLIB/Render/Config/Limits.hpp>
#include <BLIB/Render/Descriptors/DescriptorSetInstanceCache.hpp>
#include <BLIB/Render/Events/RenderPassInvalidated.hpp>
#include <BLIB/Render/Events/SettingsChanged.hpp>
#include <BLIB/Render/Vulkan/PipelineLayout.hpp>
#include <BLIB/Render/Vulkan/PipelineParameters.hpp>
#include <BLIB/Render/Vulkan/VulkanLayer.hpp>
#include <BLIB/Signals/Listener.hpp>
#include <BLIB/Vulkan.hpp>
#include <array>
#include <string>
#include <variant>
#include <vector>

namespace bl
{
namespace rc
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
class Pipeline : public sig::Listener<event::SettingsChanged, event::RenderPassInvalidated> {
public:
    /**
     * @brief Construct a new Pipeline
     *
     * @param renderer The renderer the pipeline will belong to
     * @param id The id of the pipeline
     * @param params The parameters to create the pipeline with
     */
    Pipeline(Renderer& renderer, std::uint32_t id, PipelineParameters&& params);

    /**
     * @brief Destroy the Pipeline and frees Vulkan resources
     *
     */
    ~Pipeline();

    /**
     * @brief Returns the layout of this pipeline
     *
     * @return The layout of this pipeline
     */
    const PipelineLayout& pipelineLayout() const;

    /**
     * @brief Returns the underlying Vulkan pipeline handle
     *
     * @param renderPassId The render pass to use to get the specific pipeline
     * @param specializationId The specialization id to use for the pipeline. 0 for none
     */
    VkPipeline rawPipeline(std::uint32_t renderPassId, std::uint32_t specializationId);

    /**
     * @brief Issues the command to bind the pipeline
     *
     * @param commandBuffer The command buffer to record into
     * @param renderPassId The current render pass
     * @param specializationId The specialization id to use for the pipeline
     */
    void bind(VkCommandBuffer commandBuffer, std::uint32_t renderPassId,
              std::uint32_t specializationId);

    /**
     * @brief Returns a new copy of PipelineParameters identical to the ones used to create this
     *        pipeline
     */
    const PipelineParameters& getCreationParameters() const;

    /**
     * @brief Returns the id of this pipeline
     */
    std::uint32_t getId() const { return id; }

private:
    std::uint32_t id;
    Renderer& renderer;
    PipelineLayout* layout;
    std::array<std::array<VkPipeline, cfg::Limits::MaxRenderPasses>,
               cfg::Limits::MaxPipelineSpecializations>
        pipelines;
    PipelineParameters createParams;

    void recreateForRenderPass(std::uint32_t rpid);
    void createForRenderPass(std::uint32_t rpid, std::uint32_t spec);

    virtual void process(const event::SettingsChanged& event) override;
    virtual void process(const event::RenderPassInvalidated& event) override;

    friend class PipelineCache;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

inline const PipelineLayout& Pipeline::pipelineLayout() const { return *layout; }

inline VkPipeline Pipeline::rawPipeline(std::uint32_t rpid, std::uint32_t spec) {
    if (!pipelines[spec][rpid]) { createForRenderPass(rpid, spec); }
    return pipelines[spec][rpid];
}

inline const PipelineParameters& Pipeline::getCreationParameters() const { return createParams; }

} // namespace vk
} // namespace rc
} // namespace bl

#endif
