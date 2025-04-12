#ifndef BLIB_RENDER_GRAPH_TASKS_FORWARDRENDEROPAQUETASK_HPP
#define BLIB_RENDER_GRAPH_TASKS_FORWARDRENDEROPAQUETASK_HPP

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
 * @brief Basic forward render task that calls scene render opaque method
 *
 * @ingroup Renderer
 */
class ForwardRenderOpaqueTask : public rg::Task {
public:
    /**
     * @brief Creates the task
     */
    ForwardRenderOpaqueTask();

    /**
     * @brief Destroys the task
     */
    virtual ~ForwardRenderOpaqueTask() = default;

private:
    SceneAsset* scene;

    virtual void create(engine::Engine& engine, Renderer& renderer, Scene* scene) override;
    virtual void onGraphInit() override;
    virtual void execute(const rg::ExecutionContext& ctx, rg::Asset* output) override;
};

} // namespace rgi
} // namespace rc
} // namespace bl

#endif
