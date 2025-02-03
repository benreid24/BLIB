#ifndef BLIB_RENDER_MATERIALS_MATERIALPIPELINE_HPP
#define BLIB_RENDER_MATERIALS_MATERIALPIPELINE_HPP

#include <BLIB/Render/Materials/MaterialPipelineSettings.hpp>
#include <BLIB/Render/Vulkan/Pipeline.hpp>

namespace bl
{
namespace rc
{
class Renderer;

namespace mat
{
/**
 * @brief Rendering pipeline for a material. Contains pipelines for each render phase
 *
 * @ingroup Renderer
 */
class MaterialPipeline {
public:
    /**
     * @brief Creates the material pipeline and each underlying Vulkan pipeline
     *
     * @param renderer The renderer instance
     * @param id The id of the material pipeline
     * @param settings The settings to create with
     */
    MaterialPipeline(Renderer& renderer, std::uint32_t id,
                     const MaterialPipelineSettings& settings);

    /**
     * @brief Destroys the material pipeline
     */
    ~MaterialPipeline() = default;

    /**
     * @brief Returns the pipeline layout
     */
    const vk::PipelineLayout& getLayout() const;

    /**
     * @brief Returns the raw Vulkan pipeline handle to be used for the given render pass and phase
     *
     * @param phase The render phase to get the pipeline for
     * @param renderPassId The render pass to get the pipeline for
     * @return The raw Vulkan pipeline handle
     */
    VkPipeline getRawPipeline(RenderPhase phase, std::uint32_t renderPassId) const;

    /**
     * @brief Binds the appropriate pipeline for the given render phase and render pass id
     *
     * @param commandBuffer The command buffer to issue commands into
     * @param phase The current render phase
     * @param renderPassId The current render pass id
     */
    void bind(VkCommandBuffer commandBuffer, RenderPhase phase, std::uint32_t renderPassId);

    /**
     * @brief Returns the id of this material pipeline
     */
    std::uint32_t getId() const;

    /**
     * @brief Returns the settings used to create this pipeline
     */
    const MaterialPipelineSettings& getSettings() const;

private:
    Renderer& renderer;
    std::uint32_t id;
    MaterialPipelineSettings settings;
    vk::Pipeline* mainPipeline;
    std::array<vk::Pipeline*, Config::MaxRenderPhases> pipelines;

    vk::Pipeline* resolvePipeline(MaterialPipelineSettings::PipelineInfo& info);
};

/////////////////////////////// INLINE FUNCTIONS /////////////////////////////////

inline std::uint32_t MaterialPipeline::getId() const { return id; }

inline const MaterialPipelineSettings& MaterialPipeline::getSettings() const { return settings; }

inline const vk::PipelineLayout& MaterialPipeline::getLayout() const {
    return mainPipeline->pipelineLayout();
}

} // namespace mat
} // namespace rc
} // namespace bl

#endif
