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
    Textured2D::create(engine.ecs(), entity(), texture);
    Viewport::create(engine.ecs(), entity());
}

void Sprite::destroy() { Drawable::destroy(); }

} // namespace draw
} // namespace render
} // namespace bl
