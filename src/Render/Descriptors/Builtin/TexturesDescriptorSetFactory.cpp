#include <BLIB/Render/Descriptors/Builtin/TexturesDescriptorSetFactory.hpp>

#include <BLIB/Render/Descriptors/Builtin/TexturesDescriptorSetInstance.hpp>
#include <BLIB/Render/Renderer.hpp>

namespace bl
{
namespace render
{
namespace ds
{
void ds::TexturesDescriptorSetFactory::init(engine::Engine&, Renderer& renderer) {
    texturePool = &renderer.texturePool();
}

std::unique_ptr<DescriptorSetInstance> TexturesDescriptorSetFactory::createDescriptorSet() const {
    return std::make_unique<TexturesDescriptorSetInstance>(*texturePool);
}

} // namespace ds
} // namespace render
} // namespace bl
