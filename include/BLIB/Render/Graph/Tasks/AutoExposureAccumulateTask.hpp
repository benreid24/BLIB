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
class FramebufferAsset;

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
    RenderTarget* target;
    Scene* scene;
    FramebufferAsset* input;
    vk::PipelineInstance pipeline;

    virtual void create(const rg::InitContext& ctx) override;
    virtual void onGraphInit() override;
    virtual void execute(const rg::ExecutionContext& ctx, rg::Asset* output) override;
    virtual void update(float dt) override;
};

} // namespace rgi
} // namespace rc
} // namespace bl

#endif
