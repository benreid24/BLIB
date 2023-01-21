#include <BLIB/Render/Resources/TexturePool.hpp>

namespace bl
{
namespace render
{
TexturePool::TexturePool(VulkanState& vs)
: vulkanState(vs)
, textures(MaxTextureCount)
, freeSlots(MaxTextureCount) {
    // TODO
}

TexturePool::~TexturePool() {
    // TODO
}

// TODO

} // namespace render
} // namespace bl
