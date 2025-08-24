#ifndef BLIB_RENDER_GRAPH_TASKS_DEFERREDCOMPOSITETASK_HPP
#define BLIB_RENDER_GRAPH_TASKS_DEFERREDCOMPOSITETASK_HPP

#include <BLIB/Graphics/Sphere.hpp>
#include <BLIB/Render/Buffers/IndexBuffer.hpp>
#include <BLIB/Render/Descriptors/Builtin/InputAttachmentInstance.hpp>
#include <BLIB/Render/Descriptors/Builtin/Scene3DInstance.hpp>
#include <BLIB/Render/Graph/Task.hpp>
#include <BLIB/Render/Vulkan/Pipeline.hpp>
#include <optional>

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
    buf::IndexBuffer3D sunRectBuffer;
    buf::IndexBuffer3D sphereBuffer;
    gfx::Sphere sphere;
    vk::Pipeline* pipeline;
    dsi::Scene3DInstance* sceneDescriptor;
    std::optional<dsi::InputAttachmentInstance> gbufferDescriptor;

    virtual void create(engine::Engine& engine, Renderer& renderer, Scene* scene) override;
    virtual void onGraphInit() override;
    virtual void execute(const rg::ExecutionContext& ctx, rg::Asset* output) override;
};

} // namespace rgi
} // namespace rc
} // namespace bl

#endif
