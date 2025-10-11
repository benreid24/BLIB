#include <BLIB/Render/Descriptors/Builtin/GlobalDataInstance.hpp>

#include <BLIB/Render/Scenes/SceneRenderContext.hpp>

namespace bl
{
namespace rc
{
namespace dsi
{
GlobalDataInstance::GlobalDataInstance(res::GlobalDescriptors& globals)
: DescriptorSetInstance(Bindless, SpeedAgnostic)
, globals(globals) {}

void GlobalDataInstance::bindForPipeline(scene::SceneRenderContext& ctx, VkPipelineLayout layout,
                                         std::uint32_t setIndex, UpdateSpeed) const {
    globals.bindDescriptors(ctx.getCommandBuffer(),
                            ctx.getPipelineBindPoint(),
                            layout,
                            setIndex,
                            ctx.targetIsRenderTexture());
}

void GlobalDataInstance::bindForObject(scene::SceneRenderContext&, VkPipelineLayout, std::uint32_t,
                                       scene::Key) const {
    // noop
}

void GlobalDataInstance::releaseObject(ecs::Entity, scene::Key) {
    // noop
}

void GlobalDataInstance::init(sr::ShaderResourceStore&) {
    // noop
}

bool GlobalDataInstance::allocateObject(ecs::Entity, scene::Key) {
    // noop
    return true;
}

void GlobalDataInstance::handleFrameStart() {
    // noop
}

} // namespace dsi
} // namespace rc
} // namespace bl
