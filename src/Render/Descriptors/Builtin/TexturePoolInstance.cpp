#include <BLIB/Render/Descriptors/Builtin/TexturePoolInstance.hpp>

namespace bl
{
namespace gfx
{
namespace ds
{
TexturePoolInstance::TexturePoolInstance(res::TexturePool& texturePool)
: DescriptorSetInstance(false)
, texturePool(texturePool) {}

void TexturePoolInstance::bindForPipeline(VkCommandBuffer commandBuffer, VkPipelineLayout layout,
                                          std::uint32_t, std::uint32_t setIndex) const {
    texturePool.bindDescriptors(commandBuffer, layout, setIndex);
}

void TexturePoolInstance::bindForObject(VkCommandBuffer, VkPipelineLayout, std::uint32_t,
                                        std::uint32_t) const {
    // noop
}

void TexturePoolInstance::releaseObject(std::uint32_t, ecs::Entity) {
    // noop
}

void TexturePoolInstance::doInit(std::uint32_t, std::uint32_t) {
    // noop
}

bool TexturePoolInstance::doAllocateObject(std::uint32_t, ecs::Entity, UpdateSpeed) {
    // noop
    return true;
}

void TexturePoolInstance::beginSync(bool) {
    // noop
}

} // namespace ds
} // namespace gfx
} // namespace bl
