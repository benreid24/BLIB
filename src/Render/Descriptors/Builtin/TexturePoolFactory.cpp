#include <BLIB/Render/Descriptors/Builtin/TexturePoolFactory.hpp>

#include <BLIB/Render/Descriptors/Builtin/TexturePoolInstance.hpp>
#include <BLIB/Render/Renderer.hpp>

namespace bl
{
namespace gfx
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

} // namespace ds
} // namespace gfx
} // namespace bl
