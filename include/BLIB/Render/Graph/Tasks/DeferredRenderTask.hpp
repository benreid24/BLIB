#ifndef BLIB_RENDER_GRAPH_TASKS_DEFERREDRENDERTASK_HPP
#define BLIB_RENDER_GRAPH_TASKS_DEFERREDRENDERTASK_HPP

#include <BLIB/Render/Graph/Assets/SceneAsset.hpp>
#include <BLIB/Render/Graph/Task.hpp>

namespace bl
{
namespace rc
{
namespace rgi
{
/**
 * @brief Deferred render task that calls scene render opaque method
 *
 * @ingroup Renderer
 */
class DeferredRenderTask : public rg::Task {
public:
    /**
     * @brief Creates the task
     */
    DeferredRenderTask();

    /**
     * @brief Destroys the task
     */
    virtual ~DeferredRenderTask() = default;

private:
    SceneAsset* scene;

    virtual void create(const rg::InitContext& ctx) override;
    virtual void onGraphInit() override;
    virtual void execute(const rg::ExecutionContext& ctx, rg::Asset* output) override;
};

} // namespace rgi
} // namespace rc
} // namespace bl

#endif
