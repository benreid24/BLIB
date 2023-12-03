#include <BLIB/Graphics/Sprite.hpp>

#include <BLIB/Engine/Engine.hpp>
#include <BLIB/Render/Overlays/Overlay.hpp>
#include <BLIB/Render/Primitives/Color.hpp>

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
    OverlayScalable::setLocalSize(component().getSize());
    component().containsTransparency = Textured::getTexture()->containsTransparency();
}

void Sprite::setTexture(rc::res::TextureRef texture) {
    Textured::setTexture(texture);
    component().setTexture(texture);
    component().containsTransparency = Textured::getTexture()->containsTransparency();
}

void Sprite::setTexture(rc::res::TextureRef texture, const sf::FloatRect region) {
    Textured::setTexture(texture);
    component().create(nullptr, texture, region);
    component().containsTransparency = Textured::getTexture()->containsTransparency();
}

void Sprite::setColor(const sf::Color& color) { component().setColor(sfcol(color)); }

void Sprite::scaleToSize(const glm::vec2& size) {
    getTransform().setScale(size / OverlayScalable::getLocalSize());
}

void Sprite::ensureLocalSizeUpdated() {}

} // namespace gfx
} // namespace bl
