#ifndef BLIB_RENDER_GRAPH_TASKS_AUTOEXPOSUREACCUMULATETASK_HPP
#define BLIB_RENDER_GRAPH_TASKS_AUTOEXPOSUREACCUMULATETASK_HPP

#include <BLIB/Render/Graph/Task.hpp>
#include <BLIB/Render/Vulkan/PipelineInstance.hpp>

namespace bl
{
namespace rc
{
namespace rgi
{
/**
 * @brief Task that kicks off the auto exposure accumulation compute pipeline
 *
 * @ingroup Renderer
 */
class AutoExposureAccumulateTask : public rg::Task {
public:
    /**
     * @brief Creates the task
     */
    AutoExposureAccumulateTask();

    /**
     * @brief Destroys the task
     */
    virtual ~AutoExposureAccumulateTask() = default;

private:
    Renderer* renderer;
    Scene* scene;
    vk::PipelineInstance pipeline;

    virtual void create(engine::Engine& engine, Renderer& renderer, Scene* scene) override;
    virtual void onGraphInit() override;
    virtual void execute(const rg::ExecutionContext& ctx, rg::Asset* output) override;
    virtual void update(float dt) override;
};

} // namespace rgi
} // namespace rc
} // namespace bl

#endif
