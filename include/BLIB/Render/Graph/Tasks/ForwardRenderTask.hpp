#ifndef BLIB_RENDER_GRAPH_TASKS_FORWARDRENDERTASK_HPP
#define BLIB_RENDER_GRAPH_TASKS_FORWARDRENDERTASK_HPP

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
 * @brief Basic forward renderer that calls into the scene render methods
 *
 * @ingroup Renderer
 */
class ForwardRenderTask : public rg::Task {
public:
    /**
     * @brief Creates a new forward renderer task
     */
    ForwardRenderTask();

    /**
     * @brief Destroys the task
     */
    virtual ~ForwardRenderTask() = default;

private:
    FramebufferAsset* output;
    SceneAsset* scene;

    virtual void create(engine::Engine& engine, Renderer& renderer, Scene* scene) override;
    virtual void onGraphInit() override;
    virtual void execute(const rg::ExecutionContext& ctx) override;
};

} // namespace rgi
} // namespace rc
} // namespace bl

#endif
