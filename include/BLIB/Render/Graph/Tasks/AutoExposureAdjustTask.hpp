#ifndef BLIB_RENDER_GRAPH_TASKS_AUTOEXPOSUREADJUSTTASK_HPP
#define BLIB_RENDER_GRAPH_TASKS_AUTOEXPOSUREADJUSTTASK_HPP

#include <BLIB/Render/Graph/Task.hpp>
#include <BLIB/Render/Vulkan/PipelineInstance.hpp>

namespace bl
{
namespace rc
{
namespace rgi
{
/**
 * @brief Task that kicks off the auto exposure adjustment compute pipeline
 *
 * @ingroup Renderer
 */
class AutoExposureAdjustTask : public rg::Task {
public:
    /**
     * @brief Creates the task
     */
    AutoExposureAdjustTask();

    /**
     * @brief Destroys the task
     */
    virtual ~AutoExposureAdjustTask() {}

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
