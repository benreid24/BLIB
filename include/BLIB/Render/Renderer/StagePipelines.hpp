#ifndef BLIB_RENDER_RENDERER_STAGEPIPELINES_HPP
#define BLIB_RENDER_RENDERER_STAGEPIPELINES_HPP

#include <BLIB/Render/Config.hpp>
#include <array>
#include <cstdint>

namespace bl
{
namespace render
{
/**
 * @brief Helper class to store and manage which pipelines a Renderable uses for each stage
 *
 * @ingroup Renderer
 */
class StagePipelines {
public:
    /**
     * @brief Represents a change in stage membership or the pipeline used for a single stage
     *
     */
    struct Diff {
        /// @brief What the diff is
        enum Type : std::uint8_t { Add = 0, Edit = 1, Remove = 2 };

        Type type;
        std::uint32_t renderStageId;
        std::uint32_t pipelineId;
    };

    /**
     * @brief Construct a new Render Stage Membership with no stagees being active
     *
     */
    StagePipelines();

    /**
     * @brief Returns whether or not there are any unapplied diffs to the membership
     *
     * @return True if there are changes to be made, false otherwise
     */
    bool hasDiff() const;

    /**
     * @brief Returns the next diff to be applied
     *
     * @return Diff The next diff to be applied. Undefined if no diffs are to be applied
     */
    Diff nextDiff();

    /**
     * @brief Applies the diff to the stage membership
     * 
     * @param diff The diff to apply
     */
    void applyDiff(const Diff& diff);

    /**
     * @brief Modifies which pipeline to use for the given render stage. Use to add to a renderstage
     *        as well as to change the pipeline for a render stage that this object is already in
     *
     * @param renderStageId The id of the renderstage to modify
     * @param pipelineId The pipeline to use for the render stage
     */
    void addOrSetStagePipeline(std::uint32_t renderStageId, std::uint32_t pipelineId);

    /**
     * @brief Removes this object from the given render stage
     *
     * @param renderStageId The render stage to remove from
     */
    void removeFromStage(std::uint32_t renderStageId);

    /**
     * @brief Creates a set of diffs to fully apply the membership to a new scene. Call this when an
     *        object is added to a scene it was not in before
     *
     */
    void prepareForNewScene();

    /**
     * @brief Returns the pipeline to be used for the given render stage
     *
     * @param renderStageId The render stage to query
     * @return std::uint32_t The pipeline to be used for the given render stage
     */
    std::uint32_t getPipelineForRenderStage(std::uint32_t renderStageId) const;

private:
    std::array<std::uint32_t, Config::Stage::Count> pipelines;
    std::array<std::uint32_t, Config::Stage::Count> next;
};

} // namespace render
} // namespace bl

#endif
