#ifndef BLIB_RENDER_MATERIALS_MATERIALPIPELINESETTINGS_HPP
#define BLIB_RENDER_MATERIALS_MATERIALPIPELINESETTINGS_HPP

#include <BLIB/Render/Config/Limits.hpp>
#include <BLIB/Render/Config/PipelineIds.hpp>
#include <BLIB/Render/RenderPhase.hpp>
#include <BLIB/Render/Vulkan/PipelineParameters.hpp>
#include <limits>

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
        NoOverride,

        /// Objects using this material pipeline will not be rendered
        NotRendered
    };

    /// Special value that indicates to use the specialization of the object being rendered
    static constexpr std::uint32_t ObjectSpecialization = std::numeric_limits<std::uint32_t>::max();

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
     * @brief Sets an override for the given render phase using fixed behavior. The default is to
     *        override shadow map phase with a fragment noop
     *
     * @param phase The render phase to set the override for
     * @param overrideBehavior The override behavior for the render phase
     * @return A reference to this object
     */
    MaterialPipelineSettings& withRenderPhasePipelineOverride(
        RenderPhase phase, PhasePipelineOverride overrideBehavior);

    /**
     * @brief Sets an override using a specific pipeline for the given render phase
     *
     * @param phase The render phase to set the override for
     * @param pipelineId The id of the pipeline to use during the given phase
     * @param specialization The specialization to use during the given phase
     * @return A reference to this object
     */
    MaterialPipelineSettings& withRenderPhasePipelineOverride(RenderPhase phase,
                                                              std::uint32_t pipelineId,
                                                              std::uint32_t specialization = 0);

    /**
     * @brief Sets an override using a specific pipeline from parameters for the given render phase
     *
     * @param phase The render phase to set the override for
     * @param params The parameters of the pipeline to override with
     * @param specialization The specialization to use during the given phase
     * @return A reference to this object
     */
    MaterialPipelineSettings& withRenderPhasePipelineOverride(RenderPhase phase,
                                                              vk::PipelineParameters* params,
                                                              std::uint32_t specialization = 0);

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
        std::uint32_t specialization;
        PhasePipelineOverride overrideBehavior;

        PipelineInfo()
        : id(cfg::PipelineIds::None)
        , specialization(0)
        , pipelineParams(nullptr)
        , overrideBehavior(PhasePipelineOverride::NoOverride) {}

        PipelineInfo(std::uint32_t id, std::uint32_t specialization)
        : id(id)
        , pipelineParams(nullptr)
        , specialization(specialization)
        , overrideBehavior(PhasePipelineOverride::NoOverride) {}

        PipelineInfo(vk::PipelineParameters* params, std::uint32_t specialization)
        : id(cfg::PipelineIds::None)
        , pipelineParams(params)
        , specialization(specialization)
        , overrideBehavior(PhasePipelineOverride::NoOverride) {}

        PipelineInfo(PhasePipelineOverride overrideBehavior)
        : id(cfg::PipelineIds::None)
        , pipelineParams(nullptr)
        , specialization(0)
        , overrideBehavior(overrideBehavior) {}

        bool operator==(const PipelineInfo& right) const {
            return id == right.id && pipelineParams == right.pipelineParams &&
                   overrideBehavior == right.overrideBehavior;
        }
    };

    PipelineInfo mainPipeline;
    std::array<PipelineInfo, cfg::Limits::MaxRenderPhases> renderPhaseOverrides;

    friend class MaterialPipeline;
};

} // namespace mat
} // namespace rc
} // namespace bl

#endif
