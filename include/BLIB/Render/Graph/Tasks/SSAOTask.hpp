#ifndef BLIB_RENDER_GRAPH_TASKS_SSAOTASK_HPP
#define BLIB_RENDER_GRAPH_TASKS_SSAOTASK_HPP

#include <BLIB/Render/Buffers/IndexBuffer.hpp>
#include <BLIB/Render/Descriptors/Builtin/InputAttachmentInstance.hpp>
#include <BLIB/Render/Events/SettingsChanged.hpp>
#include <BLIB/Render/Graph/Assets/SSAOAsset.hpp>
#include <BLIB/Render/Graph/Task.hpp>
#include <BLIB/Render/Vulkan/Pipeline.hpp>
#include <BLIB/Render/Vulkan/PipelineInstance.hpp>
#include <BLIB/Signals/Listener.hpp>

namespace bl
{
namespace rc
{
namespace ds
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
class SSAOTask
: public rg::Task
, public sig::Listener<event::SettingsChanged> {
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
    ds::SSAOShaderPayload* shaderParams;
    buf::IndexBuffer3D fullscreenRect;
    vk::PipelineInstance genPipeline;
    vk::PipelineInstance blurPipeline;
    SSAOAsset* genSurface;
    SSAOAsset* blurSurface;

    virtual void create(engine::Engine& engine, Renderer& renderer, Scene* scene) override;
    virtual void onGraphInit() override;
    virtual void execute(const rg::ExecutionContext& ctx, rg::Asset* output) override;

    virtual void process(const event::SettingsChanged& e) override;
    void genParams();
};

} // namespace rgi
} // namespace rc
} // namespace bl

#endif
