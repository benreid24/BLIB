#ifndef BLIB_RENDER_MATERIALS_MATERIALPIPELINE_HPP
#define BLIB_RENDER_MATERIALS_MATERIALPIPELINE_HPP

#include <BLIB/Render/Materials/MaterialPipelineSettings.hpp>

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
     * @param settings The settings to create with
     */
    MaterialPipeline(Renderer& renderer, const MaterialPipelineSettings& settings);

    /**
     * @brief Destroys the material pipeline
     */
    ~MaterialPipeline() = default;

    /**
     * @brief Binds the appropriate pipeline for the given render phase and render pass id
     *
     * @param commandBuffer The command buffer to issue commands into
     * @param phase The current render phase
     * @param renderPassId The current render pass id
     */
    void bind(VkCommandBuffer commandBuffer, RenderPhase phase, std::uint32_t renderPassId);

    /**
     * @brief Returns the settings used to create this pipeline
     */
    const MaterialPipelineSettings& getSettings() const;

private:
    Renderer& renderer;
    MaterialPipelineSettings settings;
    vk::Pipeline* mainPipeline;
    std::array<vk::Pipeline*, Config::MaxRenderPhases> pipelines;

    vk::Pipeline* resolvePipeline(MaterialPipelineSettings::PipelineInfo& info);
};

/////////////////////////////// INLINE FUNCTIONS /////////////////////////////////

const MaterialPipelineSettings& MaterialPipeline::getSettings() const { return settings; }

} // namespace mat
} // namespace rc
} // namespace bl

#endif
