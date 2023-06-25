#include <BLIB/Render/Drawables/Sprite.hpp>

#include <BLIB/Engine/Engine.hpp>
#include <BLIB/Render/Overlays/Overlay.hpp>

namespace bl
{
namespace gfx
{
namespace draw
{
Sprite::Sprite(engine::Engine& engine, res::TextureRef texture, const sf::FloatRect& region) {
    create(engine, texture, region);
}

Sprite::Sprite(engine::Engine& engine, const vk::PerFrame<res::TextureRef>& textures,
               const sf::FloatRect& region) {
    create(engine, textures, region);
}

void Sprite::create(engine::Engine& engine, res::TextureRef texture, const sf::FloatRect& region) {
    Drawable::create(engine, engine.renderer(), texture, region);
    Textured::create(engine.ecs(), entity(), texture);
    OverlayScalable::create(engine, entity());
    OverlayScalable::setLocalSize(component().getSize()); // TODO - need to sync maybe
}

void Sprite::create(engine::Engine& engine, const vk::PerFrame<res::TextureRef>& textures,
                    const sf::FloatRect& region) {
    Drawable::create(engine, engine.renderer(), textures.current(), region);
    Textured::create(engine.ecs(), entity(), textures);
    OverlayScalable::create(engine, entity());
    OverlayScalable::setLocalSize(component().getSize()); // TODO - need to sync maybe
}

} // namespace draw
} // namespace gfx
} // namespace bl
