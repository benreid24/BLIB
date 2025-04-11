#ifndef BLIB_RENDER_GRAPH_TASKS_BLOOMTASK_HPP
#define BLIB_RENDER_GRAPH_TASKS_BLOOMTASK_HPP

#include <BLIB/Render/Graph/Assets/BloomAssets.hpp>
#include <BLIB/Render/Graph/Task.hpp>

namespace bl
{
namespace rc
{
namespace rgi
{
class BloomTask : public rg::Task {
public:
    virtual ~BloomTask() = default;

private:
    FramebufferAsset* input;
    BloomColorAttachmentPair* output;

    virtual void create(engine::Engine& engine, Renderer& renderer, Scene* scene) override;
    virtual void onGraphInit() override;
    virtual void execute(const rg::ExecutionContext& ctx) override;
    virtual void update(float dt) override;
};

} // namespace rgi
} // namespace rc
} // namespace bl

#endif
