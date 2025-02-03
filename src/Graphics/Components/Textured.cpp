#include <BLIB/Graphics/Components/Textured.hpp>

#include <BLIB/Render/Renderer.hpp>

namespace bl
{
namespace gfx
{
namespace bcom
{
void Textured::create(rc::Renderer& r, com::MaterialInstance* material,
                      const rc::res::TextureRef& texture) {
    renderer = &r;
    handle   = material;
    handle->setMaterial(r.materialPool().getOrCreateFromTexture(texture));
}

void Textured::setTexture(const rc::res::TextureRef& t) {
    handle->setMaterial(renderer->materialPool().getOrCreateFromTexture(t));
}

} // namespace bcom
} // namespace gfx
} // namespace bl
