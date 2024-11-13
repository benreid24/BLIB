#include <BLIB/Graphics/Sprite.hpp>

#include <BLIB/Engine/Engine.hpp>
#include <BLIB/Render/Overlays/Overlay.hpp>

namespace bl
{
namespace gfx
{
Sprite::Sprite(engine::World& world, rc::res::TextureRef texture, const sf::FloatRect& region) {
    create(world, texture, region);
}

Sprite::Sprite(engine::World& world, ecs::Entity existing, rc::res::TextureRef texture,
               const sf::FloatRect& region) {
    create(world, existing, texture, region);
}

void Sprite::create(engine::World& world, rc::res::TextureRef texture,
                    const sf::FloatRect& region) {
    Drawable::create(world, world.engine().renderer(), texture, region);
    Textured::create(world.engine().ecs(), entity(), texture);
    OverlayScalable::create(world.engine(), entity());
    OverlayScalable::setLocalSize(component().getSize());
    component().setContainsTransparency(Textured::getTexture()->containsTransparency());
}

void Sprite::create(engine::World& world, ecs::Entity existing, rc::res::TextureRef texture,
                    const sf::FloatRect& region) {
    Drawable::createComponentOnly(world, existing, world.engine().renderer(), texture, region);
    Textured::create(world.engine().ecs(), entity(), texture);
    OverlayScalable::create(world.engine(), entity());
    OverlayScalable::setLocalSize(component().getSize());
    component().setContainsTransparency(Textured::getTexture()->containsTransparency());
}

void Sprite::setTexture(rc::res::TextureRef texture, bool reset) {
    Textured::setTexture(texture);
    component().setTexture(texture, reset);
    component().setContainsTransparency(Textured::getTexture()->containsTransparency());
    if (reset) { OverlayScalable::setLocalSize(component().getSize()); }
}

void Sprite::setTexture(rc::res::TextureRef texture, const sf::FloatRect region) {
    Textured::setTexture(texture);
    component().create(nullptr, texture, region);
    component().setContainsTransparency(Textured::getTexture()->containsTransparency());
    OverlayScalable::setLocalSize(component().getSize());
}

void Sprite::setTextureSource(const sf::FloatRect& src) { component().setTextureSource(src); }

void Sprite::setColor(const rc::Color& color) { component().setColor(color); }

void Sprite::scaleToSize(const glm::vec2& size) {
    getTransform().setScale(size / OverlayScalable::getLocalSize());
}

void Sprite::ensureLocalSizeUpdated() {}

} // namespace gfx
} // namespace bl
