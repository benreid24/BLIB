#include <BLIB/Render/Resources/MaterialPool.hpp>

namespace bl
{
namespace rc
{
namespace res
{
MaterialPool::MaterialPool(Renderer& renderer)
: renderer(renderer) {}

MaterialRef MaterialPool::getOrCreateFromTexture(const res::TextureRef& texture) {
    for (auto it = materials.begin(); it != materials.end(); ++it) {
        if (it->getTexture().id() == texture.id()) { return it.makeRef(); }
    }
    return materials.emplace(texture);
}

} // namespace res
} // namespace rc
} // namespace bl
