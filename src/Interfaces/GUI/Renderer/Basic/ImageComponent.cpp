#include <BLIB/Interfaces/GUI/Renderer/Basic/ImageComponent.hpp>

#include <BLIB/Engine/Engine.hpp>
#include <BLIB/Interfaces/GUI/Elements/Image.hpp>

namespace bl
{
namespace gui
{
namespace defcoms
{
ImageComponent::ImageComponent()
: Component(HighlightState::IgnoresMouse)
, src(nullptr)
, localPos(0.f, 0.f) {}

void ImageComponent::setVisible(bool v) { image.setHidden(!v); }

void ImageComponent::onElementUpdated() {
    Image& owner = getOwnerAs<Image>();
    if (src != &owner.getTexture()) {
        image.setTexture(engine->renderer().texturePool().getOrLoadTexture(owner.getTexture()));
    }

    const sf::Vector2f pos = localPos + owner.getOffset();
    image.getTransform().setPosition({pos.x, pos.y});
    image.getTransform().setScale({owner.getScale().x, owner.getScale().y});
}

void ImageComponent::onRenderSettingChange() {
    image.setColor(getOwnerAs<Image>().getRenderSettings().fillColor.value_or(sf::Color::White));
}

ecs::Entity ImageComponent::getEntity() const { return image.entity(); }

void ImageComponent::doCreate(engine::World& world, rdr::Renderer&) {
    engine       = &world.engine();
    Image& owner = getOwnerAs<Image>();
    image.create(world,
                 world.engine().renderer().texturePool().getOrLoadTexture(owner.getTexture()));
    src = &owner.getTexture();
}

void ImageComponent::doSceneAdd(rc::Overlay* overlay) {
    image.addToScene(overlay, rc::UpdateSpeed::Static);
}

void ImageComponent::doSceneRemove() { image.removeFromScene(); }

void ImageComponent::handleAcquisition() { handleMove(); }

void ImageComponent::handleMove() {
    Image& owner           = getOwnerAs<Image>();
    localPos               = owner.getLocalPosition();
    const sf::Vector2f pos = localPos + owner.getOffset();
    image.getTransform().setPosition({pos.x, pos.y});
}

} // namespace defcoms
} // namespace gui
} // namespace bl
