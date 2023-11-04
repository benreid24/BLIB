#include <BLIB/Interfaces/GUI/Renderer/Components/ImageComponent.hpp>

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

    const sf::Vector2f pos = localPos + owner.getLocalPosition();
    image.getTransform().setPosition({pos.x, pos.y});
    image.getTransform().setScale({owner.getScale().x, owner.getScale().y});
}

void ImageComponent::onRenderSettingChange() {
    // noop
}

ecs::Entity ImageComponent::getEntity() const { return image.entity(); }

void ImageComponent::doCreate(engine::Engine& e, rdr::Renderer&, Component*, Component&) {
    engine       = &e;
    Image& owner = getOwnerAs<Image>();
    image.create(e, e.renderer().texturePool().getOrLoadTexture(owner.getTexture()));
    src = &owner.getTexture();
}

void ImageComponent::doSceneAdd(rc::Overlay* overlay) {
    image.addToScene(overlay, rc::UpdateSpeed::Static);
}

void ImageComponent::doSceneRemove() { image.removeFromScene(); }

void ImageComponent::handleAcquisition(const sf::Vector2f& posFromParent, const sf::Vector2f&,
                                       const sf::Vector2f&) {
    handleMove(posFromParent, {});
}

void ImageComponent::handleMove(const sf::Vector2f& posFromParent, const sf::Vector2f&) {
    Image& owner           = getOwnerAs<Image>();
    localPos               = posFromParent;
    const sf::Vector2f pos = localPos + owner.getLocalPosition();
    image.getTransform().setPosition({pos.x, pos.y});
}

} // namespace defcoms
} // namespace gui
} // namespace bl
