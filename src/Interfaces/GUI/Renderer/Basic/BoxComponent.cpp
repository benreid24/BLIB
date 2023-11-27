#include <BLIB/Interfaces/GUI/Renderer/Basic/BoxComponent.hpp>

#include <BLIB/Interfaces/GUI/Elements/Box.hpp>
#include <BLIB/Render/Primitives/Color.hpp>

namespace bl
{
namespace gui
{
namespace defcoms
{
BoxComponent::BoxComponent()
: ShapeBatchProvider(HighlightState::IgnoresMouse) {}

void BoxComponent::setVisible(bool v) {
    if (isEnabled()) { batch.setHidden(!v); }
    else {
        dummy.setHidden(!v);
        if (v && !batchRect.isCreated()) {
            Box& owner = getOwnerAs<Box>();
            batchRect.create(getEngine(),
                             ShapeBatchProvider::findProvider(this)->getShapeBatch(),
                             {owner.getAcquisition().width, owner.getAcquisition().height});
            onAcquisition();
            onRenderSettingChange();
        }
        else if (!v && batchRect.isCreated()) { batchRect.remove(); }
    }
}

void BoxComponent::onElementUpdated() {
    Box& owner = getOwnerAs<Box>();
    if (owner.isViewConstrained() != isEnabled()) {
        BL_LOG_ERROR << "Cannot change Box::constrainView once component is created";
    }
}

void BoxComponent::onRenderSettingChange() {
    Box& owner                     = getOwnerAs<Box>();
    const RenderSettings& settings = owner.getRenderSettings();
    batchRect.setFillColor(bl::sfcol(settings.fillColor.value_or(sf::Color::Transparent)));
    batchRect.setOutlineColor(bl::sfcol(settings.outlineColor.value_or(sf::Color::Transparent)));
    batchRect.setOutlineThickness(-settings.outlineThickness.value_or(0.f));
}

ecs::Entity BoxComponent::getEntity() const {
    return isEnabled() ? batch.entity() : dummy.entity();
}

void BoxComponent::doCreate(engine::Engine& engine, rdr::Renderer&) {
    Box& owner = getOwnerAs<Box>();
    setEnabled(owner.isViewConstrained());
    if (owner.isViewConstrained()) { batch.create(engine, 128); }
    else { dummy.create(engine); }
    // TODO - fix scissor for enabled mode
    batchRect.create(engine,
                     ShapeBatchProvider::findProvider(this)->getShapeBatch(),
                     {owner.getAcquisition().width, owner.getAcquisition().height});
}

void BoxComponent::doSceneAdd(rc::Overlay* overlay) {
    if (isEnabled()) { batch.addToScene(overlay, rc::UpdateSpeed::Static); }
}

void BoxComponent::doSceneRemove() {
    if (isEnabled()) { batch.removeFromScene(); }
}

void BoxComponent::handleAcquisition() {
    Element& owner = getOwnerAs<Element>();
    batchRect.setSize({owner.getAcquisition().width, owner.getAcquisition().height});
    if (!isEnabled()) { dummy.setSize(batchRect.getSize()); }
    handleMove();
}

void BoxComponent::handleMove() {
    Element& owner = getOwnerAs<Element>();
    batchRect.getLocalTransform().setPosition(ShapeBatchProvider::determineOffset(this));
    if (isEnabled()) {
        batch.getTransform().setPosition({owner.getLocalPosition().x, owner.getLocalPosition().y});
    }
    else {
        dummy.getTransform().setPosition({owner.getLocalPosition().x, owner.getLocalPosition().y});
    }
}

} // namespace defcoms
} // namespace gui
} // namespace bl
