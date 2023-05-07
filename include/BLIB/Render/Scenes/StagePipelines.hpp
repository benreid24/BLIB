#ifndef BLIB_RENDER_RENDERER_STAGEPIPELINES_HPP
#define BLIB_RENDER_RENDERER_STAGEPIPELINES_HPP

#include <BLIB/Render/Config.hpp>
#include <array>
#include <cstdint>

namespace bl
{
namespace render
{
namespace scene
{
/**
 * @brief Typedef that represents pipelines used to render an object in each Scene stage it belongs
 *        to. Stages with Config::SceneObjectStage::None are not used for the object
 *
 * @ingroup Renderer
 */
using StagePipelines = std::array<std::uint32_t, Config::SceneObjectStage::Count>;

/**
 * @brief Helper class to create stage pipeline configs in a convenient manner
 *
 * @ingroup Renderer
 */
class StagePipelineBuilder {
public:
    /**
     * @brief Creates an empty StagePipelines config
     */
    StagePipelineBuilder() { value.fill(Config::PipelineIds::None); }

    /**
     * @brief Sets the pipeline for the given renderer stage
     *
     * @param stage The renderer stage to set the pipeline for
     * @param pipelineId The pipeline to use for the given stage
     * @return A reference to this builder
     */
    StagePipelineBuilder& withPipeline(std::uint32_t stage, std::uint32_t pipelineId) {
        value[stage] = pipelineId;
        return *this;
    }

    /**
     * @brief Returns the created stage pipeline config
     */
    const StagePipelines& build() const { return value; }

private:
    StagePipelines value;
};

} // namespace scene
} // namespace render
} // namespace bl

#endif
