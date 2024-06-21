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

Sprite::Sprite(engine::Engine& engine, ecs::Entity existing, rc::res::TextureRef texture,
               const sf::FloatRect& region) {
    create(engine, existing, texture, region);
}

void Sprite::create(engine::Engine& engine, rc::res::TextureRef texture,
                    const sf::FloatRect& region) {
    Drawable::create(engine, engine.renderer(), texture, region);
    Textured::create(engine.ecs(), entity(), texture);
    OverlayScalable::create(engine, entity());
    OverlayScalable::setLocalSize(component().getSize());
    component().containsTransparency = Textured::getTexture()->containsTransparency();
}

void Sprite::create(engine::Engine& engine, ecs::Entity existing, rc::res::TextureRef texture,
                    const sf::FloatRect& region) {
    Drawable::createComponentOnly(engine, existing, engine.renderer(), texture, region);
    Textured::create(engine.ecs(), entity(), texture);
    OverlayScalable::create(engine, entity());
    OverlayScalable::setLocalSize(component().getSize());
    component().containsTransparency = Textured::getTexture()->containsTransparency();
}

void Sprite::setTexture(rc::res::TextureRef texture, bool reset) {
    Textured::setTexture(texture);
    component().setTexture(texture, reset);
    component().containsTransparency = Textured::getTexture()->containsTransparency();
}

void Sprite::setTexture(rc::res::TextureRef texture, const sf::FloatRect region) {
    Textured::setTexture(texture);
    component().create(nullptr, texture, region);
    component().containsTransparency = Textured::getTexture()->containsTransparency();
    OverlayScalable::setLocalSize(component().getSize());
}

void Sprite::setTextureSource(const sf::FloatRect& src) { component().setTextureSource(src); }

void Sprite::setColor(const sf::Color& color) { component().setColor(sfcol(color)); }

void Sprite::scaleToSize(const glm::vec2& size) {
    getTransform().setScale(size / OverlayScalable::getLocalSize());
}

void Sprite::ensureLocalSizeUpdated() {}

} // namespace gfx
} // namespace bl
