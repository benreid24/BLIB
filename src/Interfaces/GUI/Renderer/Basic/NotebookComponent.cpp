#include <BLIB/Interfaces/GUI/Renderer/Basic/NotebookComponent.hpp>

#include <BLIB/Interfaces/GUI/Elements/Notebook.hpp>
#include <BLIB/Render/Primitives/Color.hpp>

namespace bl
{
namespace gui
{
namespace defcoms
{
NotebookComponent::NotebookComponent()
: Component(HighlightState::IgnoresMouse) {}

void NotebookComponent::setVisible(bool v) { box.setHidden(!v); }

void NotebookComponent::onElementUpdated() {
    // noop
}

void NotebookComponent::onRenderSettingChange() {
    const RenderSettings& settings = getOwnerAs<Element>().getRenderSettings();
    box.setFillColor(bl::sfcol(settings.fillColor.value_or(sf::Color(120, 120, 120))));
    box.setOutlineColor(bl::sfcol(settings.outlineColor.value_or(sf::Color::Black)));
    box.setOutlineThickness(-settings.outlineThickness.value_or(1.f));
}

ecs::Entity NotebookComponent::getEntity() const { return box.entity(); }

void NotebookComponent::doCreate(engine::Engine& engine, rdr::Renderer&, Component*, Component&) {
    Element& owner = getOwnerAs<Element>();
    box.create(engine, {owner.getAcquisition().width, owner.getAcquisition().height});
    box.getOverlayScaler().setScissorMode(com::OverlayScaler::ScissorSelfConstrained);
}

void NotebookComponent::doSceneAdd(rc::Overlay* overlay) {
    box.addToScene(overlay, rc::UpdateSpeed::Static);
}

void NotebookComponent::doSceneRemove() { box.removeFromScene(); }

void NotebookComponent::handleAcquisition(const sf::Vector2f& posFromParent, const sf::Vector2f&,
                                          const sf::Vector2f& size) {
    Notebook& owner            = getOwnerAs<Notebook>();
    const sf::FloatRect& acq   = owner.getAcquisition();
    const sf::FloatRect tabAcq = owner.getTabAcquisition();

    box.setSize({size.x, size.y});
    box.getTransform().setPosition({posFromParent.x, posFromParent.y});
}

void NotebookComponent::handleMove(const sf::Vector2f& posFromParent, const sf::Vector2f&) {
    box.getTransform().setPosition({posFromParent.x, posFromParent.y});
}

} // namespace defcoms
} // namespace gui
} // namespace bl
