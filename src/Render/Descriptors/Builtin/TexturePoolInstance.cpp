#include <BLIB/Render/Descriptors/Builtin/TexturePoolInstance.hpp>

#include <BLIB/Render/Scenes/SceneRenderContext.hpp>

namespace bl
{
namespace gfx
{
namespace ds
{
TexturePoolInstance::TexturePoolInstance(res::TexturePool& texturePool)
: DescriptorSetInstance(Bindless, SpeedAgnostic)
, texturePool(texturePool) {}

void TexturePoolInstance::bindForPipeline(scene::SceneRenderContext& ctx, VkPipelineLayout layout,
                                          std::uint32_t setIndex, UpdateSpeed) const {
    texturePool.bindDescriptors(
        ctx.getCommandBuffer(), layout, setIndex, ctx.targetIsRenderTexture());
}

void TexturePoolInstance::bindForObject(scene::SceneRenderContext&, VkPipelineLayout, std::uint32_t,
                                        scene::Key) const {
    // noop
}

void TexturePoolInstance::releaseObject(ecs::Entity, scene::Key) {
    // noop
}

void TexturePoolInstance::init(DescriptorComponentStorageCache&) {
    // noop
}

bool TexturePoolInstance::allocateObject(ecs::Entity, scene::Key) {
    // noop
    return true;
}

void TexturePoolInstance::handleFrameStart() {
    // noop
}

} // namespace ds
} // namespace gfx
} // namespace bl
