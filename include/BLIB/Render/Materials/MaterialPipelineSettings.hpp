#ifndef BLIB_RENDER_MATERIALS_MATERIALPIPELINESETTINGS_HPP
#define BLIB_RENDER_MATERIALS_MATERIALPIPELINESETTINGS_HPP

#include <BLIB/Render/RenderPhase.hpp>
#include <BLIB/Render/Vulkan/PipelineParameters.hpp>

namespace bl
{
namespace rc
{
/// Namespace containing classes and functionality related to materials
namespace mat
{
class MaterialPipeline;

/**
 * @brief Settings that define the behavior of a material pipeline
 *
 * @ingroup Renderer
 */
class MaterialPipelineSettings {
public:
    /**
     * @brief Creates default settings and uses the given pipeline
     *
     * @param pipelineId The id of the Vulkan pipeline the material should use
     */
    MaterialPipelineSettings(std::uint32_t pipelineId);

    /**
     * @brief Creates default settings and uses the given pipeline
     *
     * @param pipelineParams The parameters of the pipeline to use. Must remain valid until created
     */
    MaterialPipelineSettings(vk::PipelineParameters* pipelineParams);

    /**
     * @brief Copies from the given settings
     *
     * @param copy The settings to copy
     */
    MaterialPipelineSettings(const MaterialPipelineSettings& copy);

    /**
     * @brief Sets the pipeline to use
     *
     * @param pipelineId The id of the pipeline to us
     * @return A reference to this object
     */
    MaterialPipelineSettings& withPipeline(std::uint32_t pipelineId);

    /**
     * @brief Sets the pipeline to use
     *
     * @param pipelineParams The parameters of the pipeline to use
     * @return A reference to this object
     */
    MaterialPipelineSettings& withPipeline(vk::PipelineParameters* pipelineParams);

    /**
     * @brief Sets which render phases the material should render in
     *
     * @param phase The phases to render in
     * @return A reference to this object
     */
    MaterialPipelineSettings& withRenderPhase(RenderPhase phase);

    /**
     * @brief Sets the fragment shader to use for shadow rendering. Uses the noop shader by default
     *
     * @param path The path of the shader to use
     * @param entrypoint The entrypoint of the shader to use
     * @return A reference to this object
     */
    MaterialPipelineSettings& withShadowFragmentShaderOverride(
        const std::string& path, const std::string& entrypoint = "main");

    /**
     * @brief Validates the settings and returns an rvalue reference to this object
     */
    MaterialPipelineSettings&& build();

private:
    struct ShaderInfo {
        std::string path;
        std::string entrypoint;

        ShaderInfo();
    };

    std::uint32_t pipelineId;
    vk::PipelineParameters* pipelineParams;

    RenderPhase phases;
    ShaderInfo shadowFragmentShaderOverride;

    friend class MaterialPipeline;
};

} // namespace mat
} // namespace rc
} // namespace bl

#endif
