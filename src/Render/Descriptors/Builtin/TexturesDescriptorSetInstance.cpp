#include <BLIB/Render/Descriptors/Builtin/TexturesDescriptorSetInstance.hpp>

namespace bl
{
namespace render
{
namespace ds
{
TexturesDescriptorSetInstance::TexturesDescriptorSetInstance(TexturePool& texturePool)
: DescriptorSetInstance(false)
, texturePool(texturePool) {}

void TexturesDescriptorSetInstance::bindForPipeline(VkCommandBuffer commandBuffer,
                                                    VkPipelineLayout layout, std::uint32_t,
                                                    std::uint32_t setIndex) const {
    texturePool.bindDescriptors(commandBuffer, layout, setIndex);
}

void TexturesDescriptorSetInstance::bindForObject(VkCommandBuffer, VkPipelineLayout, std::uint32_t,
                                                  std::uint32_t) const {
    // noop
}

void TexturesDescriptorSetInstance::releaseObject(std::uint32_t, ecs::Entity) {
    // noop
}

void TexturesDescriptorSetInstance::doInit(std::uint32_t, std::uint32_t) {
    // noop
}

bool TexturesDescriptorSetInstance::doAllocateObject(std::uint32_t, ecs::Entity,
                                                     SceneObject::UpdateSpeed) {
    // noop
    return true;
}

void TexturesDescriptorSetInstance::beginSync(bool) {
    // noop
}

} // namespace ds
} // namespace render
} // namespace bl
