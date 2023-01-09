#ifndef BLIB_RENDER_RENDERABLES_RENDERPASSMEMBERSHIP_HPP
#define BLIB_RENDER_RENDERABLES_RENDERPASSMEMBERSHIP_HPP

#include <BLIB/Render/Config.hpp>
#include <array>
#include <cstdint>

namespace bl
{
namespace render
{
/**
 * @brief Helper class to store and manage a Renderable's membership in a Scene's render passes.
 *        Stores membership changes as a compact sequence of diffs to allow for efficient changing
 *        of object batching if pipelines and passes are changed
 *
 * @ingroup Renderer
 */
class RenderPassMembership {
public:
    /**
     * @brief Represents a change in pass membership or the pipeline used for a single pass
     *
     */
    struct Diff {
        /// @brief What the diff is
        enum Type : std::uint8_t { Add = 0, Edit = 1, Remove = 2 };

        Type type;
        std::uint32_t renderPassId;
        std::uint32_t pipelineId;
    };

    /**
     * @brief Construct a new Render Pass Membership with no passes being active
     *
     */
    RenderPassMembership();

    /**
     * @brief Returns whether or not there are any unapplied diffs to the membership
     *
     * @return True if there are changes to be made, false otherwise
     */
    bool hasDiff() const;

    /**
     * @brief Returns the next diff to be applied and applies it to this object before returning
     *
     * @return Diff The next diff to be applied. Undefined if no diffs are to be applied
     */
    Diff nextDiff();

    /**
     * @brief Modifies which pipeline to use for the given render pass. Use to add to a renderpass
     *        as well as to change the pipeline for a render pass that this object is already in
     *
     * @param renderPassId The id of the renderpass to modify
     * @param pipelineId The pipeline to use for the render pass
     */
    void addOrSetPassPipeline(std::uint32_t renderPassId, std::uint32_t pipelineId);

    /**
     * @brief Removes this object from the given render pass
     *
     * @param renderPassId The render pass to remove from
     */
    void removeFromPass(std::uint32_t renderPassId);

    /**
     * @brief Creates a set of diffs to fully apply the membership to a new scene. Call this when an
     *        object is added to a scene it was not in before
     *
     */
    void prepareForNewScene();

    /**
     * @brief Returns the pipeline to be used for the given render pass
     *
     * @param renderPassId The render pass to query
     * @return std::uint32_t The pipeline to be used for the given render pass
     */
    std::uint32_t getPipelineForRenderPass(std::uint32_t renderPassId) const;

private:
    std::array<std::uint32_t, Config::RenderPassIds::Count> pipelines;
    std::array<std::uint32_t, Config::RenderPassIds::Count> next;
};

} // namespace render
} // namespace bl

#endif
