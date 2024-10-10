#include <BLIB/Interfaces/GUI/Renderer/Basic/CanvasComponent.hpp>

#include <BLIB/Engine/Engine.hpp>
#include <BLIB/Interfaces/GUI/Elements/Canvas.hpp>

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

    if (owner.getScene().isValid() && owner.getScene().get() != texture->getCurrentScene()) {
        texture->popScene();
        texture->pushScene(owner.getScene());
        if (owner.getCamera()) { texture->setCamera(std::move(owner.getCamera())); }
    }
}

void CanvasComponent::onRenderSettingChange() {
    Canvas& owner = getOwnerAs<Canvas>();
    texture->setClearColor(owner.getClearColor());
}

ecs::Entity CanvasComponent::getEntity() const { return img.entity(); }

rc::vk::RenderTexture& CanvasComponent::getRenderTexture() { return *texture; }

void CanvasComponent::doCreate(engine::World& world, rdr::Renderer&) {
    Canvas& owner = getOwnerAs<Canvas>();

    texture = world.engine().renderer().createRenderTexture(
        {owner.getTextureSize().x, owner.getTextureSize().y});
    if (owner.getScene().isValid()) {
        texture->pushScene(owner.getScene());
        if (owner.getCamera()) { texture->setCamera(std::move(owner.getCamera())); }
    }
    texture->setClearColor(owner.getClearColor());

    img.create(world, texture->getTexture());
}

void CanvasComponent::doSceneAdd(rc::Overlay* overlay) {
    img.addToScene(overlay, rc::UpdateSpeed::Static);
}

void CanvasComponent::doSceneRemove() { img.removeFromScene(); }

void CanvasComponent::handleAcquisition() {
    pos = getOwnerAs<Element>().getLocalPosition();
    onElementUpdated();
}

void CanvasComponent::handleMove() {
    pos = getOwnerAs<Element>().getLocalPosition();
    onElementUpdated();
}

} // namespace defcoms
} // namespace gui
} // namespace bl
