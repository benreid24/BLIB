#ifndef BLIB_RENDER_GRAPH_TASKS_DEFERREDCOMPOSITETASK_HPP
#define BLIB_RENDER_GRAPH_TASKS_DEFERREDCOMPOSITETASK_HPP

#include <BLIB/Render/Buffers/IndexBuffer.hpp>
#include <BLIB/Render/Graph/Task.hpp>
#include <BLIB/Render/Vulkan/Pipeline.hpp>

namespace bl
{
namespace rc
{
namespace rgi
{
/**
 * @brief Deferred render task that composites the GBuffer and renders transparent objects
 *
 * @ingroup Renderer
 */
class DeferredCompositeTask : public rg::Task {
public:
    /**
     * @brief Creates the task
     */
    DeferredCompositeTask();

    /**
     * @brief Destroys the task
     */
    virtual ~DeferredCompositeTask() = default;

private:
    Renderer* renderer;
    Scene* scene;
    buf::IndexBuffer indexBuffer;
    vk::Pipeline* pipeline;

    virtual void create(engine::Engine& engine, Renderer& renderer, Scene* scene) override;
    virtual void onGraphInit() override;
    virtual void execute(const rg::ExecutionContext& ctx, rg::Asset* output) override;
};

} // namespace rgi
} // namespace rc
} // namespace bl

#endif
