#include <BLIB/Render/Descriptors/Builtin/TexturePoolFactory.hpp>

#include <BLIB/Render/Descriptors/Builtin/TexturePoolInstance.hpp>
#include <BLIB/Render/Renderer.hpp>

namespace bl
{
namespace rc
{
namespace ds
{
void ds::TexturePoolFactory::init(engine::Engine&, Renderer& renderer) {
    texturePool         = &renderer.texturePool();
    descriptorSetLayout = renderer.texturePool().getDescriptorLayout();
}

std::unique_ptr<DescriptorSetInstance> TexturePoolFactory::createDescriptorSet() const {
    return std::make_unique<TexturePoolInstance>(*texturePool);
}

std::type_index TexturePoolFactory::creates() const { return typeid(TexturePoolInstance); }

} // namespace ds
} // namespace rc
} // namespace bl
