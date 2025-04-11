#ifndef BLIB_RENDER_GRAPH_TASKS_POSTPROCESS3D_HPP
#define BLIB_RENDER_GRAPH_TASKS_POSTPROCESS3D_HPP

#include <BLIB/Render/Buffers/IndexBuffer.hpp>
#include <BLIB/Render/Descriptors/Builtin/ColorAttachmentInstance.hpp>
#include <BLIB/Render/Graph/Assets/FramebufferAsset.hpp>
#include <BLIB/Render/Graph/Task.hpp>
#include <BLIB/Render/Vulkan/AttachmentBufferSet.hpp>
#include <BLIB/Render/Vulkan/Pipeline.hpp>
#include <BLIB/Render/Vulkan/PipelineInstance.hpp>
#include <optional>

namespace bl
{
namespace rc
{
namespace rgi
{
/**
 * @brief Render graph task that performs post-processing for 3D scenes
 *
 * @ingroup Renderer
 */
class PostProcess3DTask : public rg::Task {
public:
    /**
     * @brief Initializes the task
     */
    PostProcess3DTask();

    /**
     * @brief Destroys the task
     */
    virtual ~PostProcess3DTask() = default;

private:
    Renderer* renderer;
    Scene* scene;
    FramebufferAsset* input;
    FramebufferAsset* output;
    buf::IndexBuffer indexBuffer;
    vk::Pipeline* pipeline;
    std::optional<ds::ColorAttachmentInstance> colorAttachmentSet;
    std::optional<ds::ColorAttachmentInstance> bloomAttachmentSet;
    std::optional<vk::AttachmentBufferSet<1>> dummyBloomBuffer;

    virtual void create(engine::Engine& engine, Renderer& renderer, Scene* scene) override;
    virtual void onGraphInit() override;
    virtual void execute(const rg::ExecutionContext& ctx) override;
    virtual void update(float dt) override;
};

} // namespace rgi
} // namespace rc
} // namespace bl

#endif
