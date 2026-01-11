#ifndef BLIB_RENDER_GRAPH_TASKS_SSAOTASK_HPP
#define BLIB_RENDER_GRAPH_TASKS_SSAOTASK_HPP

#include <BLIB/Render/Buffers/IndexBuffer.hpp>
#include <BLIB/Render/Graph/Assets/SSAOAsset.hpp>
#include <BLIB/Render/Graph/Task.hpp>
#include <BLIB/Render/Vulkan/Pipeline.hpp>
#include <BLIB/Render/Vulkan/PipelineInstance.hpp>

namespace bl
{
namespace rc
{
namespace dsi
{
struct SSAOShaderPayload;
}
namespace rgi
{

/**
 * @brief Task that performs SSAO on 3d scenes
 *
 * @ingroup Renderer
 */
class SSAOTask : public rg::Task {
public:
    /**
     * @brief Creates the task
     */
    SSAOTask();

    /**
     * @brief Destroys the task
     */
    virtual ~SSAOTask() = default;

private:
    Renderer* renderer;
    buf::IndexBuffer3D fullscreenRect;
    vk::PipelineInstance genPipeline;
    vk::PipelineInstance blurPipeline;
    SSAOAsset* genSurface;
    SSAOAsset* blurSurface;

    virtual void create(const rg::InitContext& ctx) override;
    virtual void onGraphInit() override;
    virtual void execute(const rg::ExecutionContext& ctx, rg::Asset* output) override;
};

} // namespace rgi
} // namespace rc
} // namespace bl

#endif
