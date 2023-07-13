#include <BLIB/Graphics/Sprite.hpp>

#include <BLIB/Engine/Engine.hpp>
#include <BLIB/Render/Overlays/Overlay.hpp>

namespace bl
{
namespace gfx
{
Sprite::Sprite(engine::Engine& engine, rc::res::TextureRef texture, const sf::FloatRect& region) {
    create(engine, texture, region);
}

void Sprite::create(engine::Engine& engine, rc::res::TextureRef texture,
                    const sf::FloatRect& region) {
    Drawable::create(engine, engine.renderer(), texture, region);
    Textured::create(engine.ecs(), entity(), texture);
    OverlayScalable::create(engine, entity());
    OverlayScalable::setLocalSize(component().getSize()); // TODO - need to sync maybe
}

} // namespace gfx
} // namespace bl
