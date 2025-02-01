#ifndef BLIB_RENDER_MATERIALS_MATERIALPIPELINESETTINGS_HPP
#define BLIB_RENDER_MATERIALS_MATERIALPIPELINESETTINGS_HPP

#include <BLIB/Render/Config.hpp>
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
    /// Represents the behavior of a pipeline override for a given render phase
    enum PhasePipelineOverride {
        /// Do not override the pipeline for this render phase
        None,

        /// Uses a derivative of the main pipeline with no fragment shader
        FragmentNoop
    };

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
     * @brief Sets an override for the given render phase using fixed behavior. The default is to
     *        override shadow map phase with a fragment noop
     *
     * @param phase The render phase to set the override for
     * @param overrideBehavior The override behavior for the render phase
     * @return A reference to this object
     */
    MaterialPipelineSettings& withPhasePipelineOverride(RenderPhase phase,
                                                        PhasePipelineOverride overrideBehavior);

    /**
     * @brief Sets an override using a specific pipeline for the given render phase
     *
     * @param phase The render phase to set the override for
     * @param pipelineId The id of the pipeline to use during the given phase
     * @return A reference to this object
     */
    MaterialPipelineSettings& withRenderPhaseShaderOverride(RenderPhase phase,
                                                            std::uint32_t pipelineId);

    /**
     * @brief Sets an override using a specific pipeline from parameters for the given render phase
     *
     * @param phase The render phase to set the override for
     * @param params The parameters of the pipeline to override with
     * @return A reference to this object
     */
    MaterialPipelineSettings& withRenderPhaseShaderOverride(RenderPhase phase,
                                                            vk::PipelineParameters* params);

    /**
     * @brief Validates the settings and returns an rvalue reference to this object
     */
    MaterialPipelineSettings&& build();

    /**
     * @brief Returns whether these settings are equal to the given settings
     *
     * @param right The settings to compare with
     * @return True if the settings are the same, false otherwise
     */
    bool operator==(const MaterialPipelineSettings& right) const;

    /**
     * @brief Returns whether these settings are not equal to the given settings
     *
     * @param right The settings to compare with
     * @return True if the settings are different, false otherwise
     */
    bool operator!=(const MaterialPipelineSettings& right) const;

private:
    struct PipelineInfo {
        std::uint32_t id;
        vk::PipelineParameters* pipelineParams;
        PhasePipelineOverride overrideBehavior;

        PipelineInfo()
        : id(Config::PipelineIds::None)
        , pipelineParams(nullptr)
        , overrideBehavior(PhasePipelineOverride::None) {}

        PipelineInfo(std::uint32_t id)
        : id(id)
        , pipelineParams(nullptr)
        , overrideBehavior(PhasePipelineOverride::None) {}

        PipelineInfo(vk::PipelineParameters* params)
        : id(Config::PipelineIds::None)
        , pipelineParams(params)
        , overrideBehavior(PhasePipelineOverride::None) {}

        PipelineInfo(PhasePipelineOverride overrideBehavior)
        : id(Config::PipelineIds::None)
        , pipelineParams(nullptr)
        , overrideBehavior(overrideBehavior) {}

        bool operator==(const PipelineInfo& right) const {
            return id == right.id && pipelineParams == right.pipelineParams &&
                   overrideBehavior == right.overrideBehavior;
        }
    };

    PipelineInfo mainPipeline;

    RenderPhase phases;
    std::array<PipelineInfo, Config::MaxRenderPhases> renderPhaseOverrides;

    friend class MaterialPipeline;
};

} // namespace mat
} // namespace rc
} // namespace bl

#endif
