#ifndef BLIB_RENDER_GRAPH_TASKS_RENDEROVERLAYTASK_HPP
#define BLIB_RENDER_GRAPH_TASKS_RENDEROVERLAYTASK_HPP

#include <BLIB/Render/Graph/Assets/FramebufferAsset.hpp>
#include <BLIB/Render/Graph/Assets/SceneAsset.hpp>
#include <BLIB/Render/Graph/Task.hpp>

namespace bl
{
namespace rc
{
namespace rgi
{
/**
 * @brief Basic render task that calls Overlay render method
 *
 * @ingroup Renderer
 */
class RenderOverlayTask : public rg::Task {
public:
    /**
     * @brief Creates the task
     *
     * @param observerIndexOverride Optional override to use for observer index
     */
    RenderOverlayTask(unsigned int* observerIndexOverride = nullptr);

    /**
     * @brief Destroys the task
     */
    virtual ~RenderOverlayTask() = default;

private:
    SceneAsset* scene;
    unsigned int* observerIndexOverride;

    virtual void create(const rg::InitContext& ctx) override;
    virtual void onGraphInit() override;
    virtual void execute(const rg::ExecutionContext& ctx, rg::Asset* output) override;
};

} // namespace rgi
} // namespace rc
} // namespace bl

#endif
