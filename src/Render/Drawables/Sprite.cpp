#include <BLIB/Render/Drawables/Sprite.hpp>

#include <BLIB/Engine/Engine.hpp>

namespace bl
{
namespace render
{
namespace draw
{
Sprite::Sprite(engine::Engine& engine, res::TextureRef texture, const sf::FloatRect& region) {
    create(engine, texture, region);
}

Sprite::~Sprite() { destroy(); }

void Sprite::create(engine::Engine& engine, res::TextureRef texture, const sf::FloatRect& region) {
    Drawable::create(engine, engine.renderer(), texture, region);
    Textured::create(engine.ecs(), entity(), texture);
    Transformable::create(engine.ecs(), entity());
    Viewport::create(engine.ecs(), entity());
}

void Sprite::destroy() { Drawable::destroy(); }

void Sprite::scaleWidthToOverlay(float w, float ow) {
    const glm::vec2& size = component().getTexture()->sizeF;
    const float ar        = size.x / size.y;
    const float xs        = w / size.x * ow;
    getTransform().setScale({xs, xs * ar});
}

void Sprite::scaleHeightToOverlay(float h, float oh) {
    const glm::vec2& size = component().getTexture()->sizeF;
    const float ar        = size.y / size.x;
    const float xy        = h / size.y * oh;
    getTransform().setScale({xy * ar, xy});
}

void Sprite::scaleToOverlay(const glm::vec2& r, const glm::vec2& os) {
    const glm::vec2& size = component().getTexture()->sizeF;
    getTransform().setScale({r.x / size.x * os.x, r.y / size.y * os.y});
}

} // namespace draw
} // namespace render
} // namespace bl
