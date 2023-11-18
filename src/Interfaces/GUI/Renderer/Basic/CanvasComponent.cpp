#include <BLIB/Interfaces/GUI/Renderer/Basic/CanvasComponent.hpp>

#include <BLIB/Engine/Engine.hpp>
#include <BLIB/Interfaces/GUI/Elements/Canvas.hpp>
#include <BLIB/Render/Primitives/Color.hpp>

namespace bl
{
namespace gui
{
namespace defcoms
{
CanvasComponent::CanvasComponent()
: CanvasComponentBase(HighlightState::IgnoresMouse)
, pos(0.f, 0.f) {}

void CanvasComponent::setVisible(bool v) { img.setHidden(!v); }

void CanvasComponent::onElementUpdated() {
    Canvas& owner            = getOwnerAs<Canvas>();
    const sf::Vector2f sfpos = pos + owner.getOffset();
    img.getTransform().setPosition({sfpos.x, sfpos.y});
    img.getTransform().setScale({owner.getScale().x, owner.getScale().y});
}

void CanvasComponent::onRenderSettingChange() {
    Canvas& owner = getOwnerAs<Canvas>();
    texture.setClearColor(sfcol(owner.getClearColor()));
}

ecs::Entity CanvasComponent::getEntity() const { return img.entity(); }

rc::vk::RenderTexture& CanvasComponent::getRenderTexture() { return texture; }

void CanvasComponent::doCreate(engine::Engine& engine, rdr::Renderer&, Component*, Component&) {
    Canvas& owner = getOwnerAs<Canvas>();

    texture.create(engine.renderer(), {owner.getTextureSize().x, owner.getTextureSize().y});
    if (owner.getScene().isValid()) { texture.setScene(owner.getScene()); }
    texture.setClearColor(sfcol(owner.getClearColor()));

    img.create(engine, texture.getTexture());
}

void CanvasComponent::doSceneAdd(rc::Overlay* overlay) {
    img.addToScene(overlay, rc::UpdateSpeed::Static);
}

void CanvasComponent::doSceneRemove() { img.removeFromScene(); }

void CanvasComponent::handleAcquisition(const sf::Vector2f& posFromParent, const sf::Vector2f&,
                                        const sf::Vector2f&) {
    pos = posFromParent;
    onElementUpdated();
}

void CanvasComponent::handleMove(const sf::Vector2f& posFromParent, const sf::Vector2f&) {
    pos = posFromParent;
    onElementUpdated();
}

} // namespace defcoms
} // namespace gui
} // namespace bl
