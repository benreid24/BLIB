#ifndef BLIB_RENDER_GRAPH_TASKS_FORWARDRENDERTRANSPARENTTASK_HPP
#define BLIB_RENDER_GRAPH_TASKS_FORWARDRENDERTRANSPARENTTASK_HPP

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
 * @brief Basic forward render task that calls scene render transparent method
 *
 * @ingroup Renderer
 */
class ForwardRenderTransparentTask : public rg::Task {
public:
    /**
     * @brief Creates the task
     */
    ForwardRenderTransparentTask();

    /**
     * @brief Destroys the task
     */
    virtual ~ForwardRenderTransparentTask() = default;

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
