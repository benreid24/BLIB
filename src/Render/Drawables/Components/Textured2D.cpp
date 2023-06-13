#include <BLIB/Render/Drawables/Components/Textured2D.hpp>

#include <BLIB/Render/Resources/Texture.hpp>

namespace bl
{
namespace render
{
namespace draw
{
namespace base
{
void Textured2D::scaleWidthToOverlay(float w, float ow) {
    const glm::vec2& size = getTexture()->sizeF;
    const float ar        = size.x / size.y;
    const float xs        = w / size.x * ow;
    getTransform().setScale({xs, xs * ar});
}

void Textured2D::scaleHeightToOverlay(float h, float oh) {
    const glm::vec2& size = getTexture()->sizeF;
    const float ar        = size.y / size.x;
    const float xy        = h / size.y * oh;
    getTransform().setScale({xy * ar, xy});
}

void Textured2D::scaleToOverlay(const glm::vec2& r, const glm::vec2& os) {
    const glm::vec2& size = getTexture()->sizeF;
    getTransform().setScale({r.x / size.x * os.x, r.y / size.y * os.y});
}

void Textured2D::create(ecs::Registry& registry, ecs::Entity ent, const res::TextureRef& t) {
    Transformable::create(registry, ent);
    Textured::create(registry, ent, t);
}

} // namespace base
} // namespace draw
} // namespace render
} // namespace bl
