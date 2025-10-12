#include <BLIB/Render/Descriptors/Builtin/SlideshowInstance.hpp>

#include <BLIB/Engine/Engine.hpp>
#include <BLIB/Render/Renderer.hpp>
#include <BLIB/Systems/Animation2DSystem.hpp>

namespace bl
{
namespace rc
{
namespace dsi
{
SlideshowInstance::SlideshowInstance(engine::Engine& engine)
: DescriptorSetInstance(Bindless, SpeedAgnostic)
, animSystem(engine.systems().getSystem<sys::Animation2DSystem>()) {}

void SlideshowInstance::init(sr::ShaderResourceStore&, sr::ShaderResourceStore&,
                             sr::ShaderResourceStore&) {
    // noop
}

void SlideshowInstance::bindForPipeline(scene::SceneRenderContext& ctx, VkPipelineLayout layout,
                                        std::uint32_t setIndex, UpdateSpeed) const {
    animSystem.bindSlideshowSet(ctx.getCommandBuffer(), layout, setIndex);
}

void SlideshowInstance::bindForObject(scene::SceneRenderContext&, VkPipelineLayout, std::uint32_t,
                                      scene::Key) const {
    // noop
}

bool SlideshowInstance::allocateObject(ecs::Entity, scene::Key) {
    // noop?
    return true;
}

void SlideshowInstance::releaseObject(ecs::Entity, scene::Key) {
    // noop
}

void SlideshowInstance::updateDescriptors() {
    // noop
}

} // namespace dsi
} // namespace rc
} // namespace bl
