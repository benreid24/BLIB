#ifndef BLIB_RENDER_GRAPH_TASKS_BLOOMTASK_HPP
#define BLIB_RENDER_GRAPH_TASKS_BLOOMTASK_HPP

#include <BLIB/Render/Buffers/IndexBuffer.hpp>
#include <BLIB/Render/Descriptors/Builtin/ColorAttachmentInstance.hpp>
#include <BLIB/Render/Graph/Assets/BloomAssets.hpp>
#include <BLIB/Render/Graph/Task.hpp>
#include <BLIB/Render/Vulkan/PipelineInstance.hpp>
#include <optional>

namespace bl
{
namespace rc
{
namespace rgi
{
/**
 * @brief Task that creates the bloom filter for the post processing task to apply
 *
 * @ingroup Renderer
 */
class BloomTask : public rg::Task {
public:
    /**
     * @brief Initializes the task
     */
    BloomTask();

    /**
     * @brief Destroys the task
     */
    virtual ~BloomTask() = default;

private:
    Renderer* renderer;
    FramebufferAsset* input;
    BloomColorAttachmentPairAsset* output;
    vk::Pipeline* filterHighlightPipeline;
    vk::Pipeline* blurPipeline;
    buf::IndexBuffer indexBuffer;
    std::optional<ds::ColorAttachmentInstance> inputAttachmentDescriptor;
    std::optional<ds::ColorAttachmentInstance> output1Descriptor;
    std::optional<ds::ColorAttachmentInstance> output2Descriptor;

    virtual void create(engine::Engine& engine, Renderer& renderer, Scene* scene) override;
    virtual void onGraphInit() override;
    virtual void execute(const rg::ExecutionContext& ctx, rg::Asset*) override;
    virtual void update(float dt) override;
};

} // namespace rgi
} // namespace rc
} // namespace bl

#endif
