#include <BLIB/Interfaces/GUI/Renderer/Components/ProgressBarComponent.hpp>

#include <BLIB/Interfaces/GUI/Elements/ProgressBar.hpp>
#include <BLIB/Render/Primitives/Color.hpp>

namespace bl
{
namespace gui
{
namespace defcoms
{
ProgressBarComponent::ProgressBarComponent()
: Component(HighlightState::IgnoresMouse) {}

void ProgressBarComponent::setVisible(bool v) { background.setHidden(!v); }

void ProgressBarComponent::onElementUpdated() {
    ProgressBar& owner             = getOwnerAs<ProgressBar>();
    const RenderSettings& settings = getOwnerAs<ProgressBar>().getRenderSettings();
    const float ot                 = settings.outlineThickness.value_or(1.f);
    const glm::vec2 bsize{owner.getAcquisition().width - ot * 2.f,
                          owner.getAcquisition().height - ot * 2.f};

    // size bar based on progress
    const bool hor = owner.getFillDirection() == ProgressBar::LeftToRight ||
                     owner.getFillDirection() == ProgressBar::RightToLeft;
    const glm::vec2 size{hor ? owner.getProgress() : 1.f, hor ? 1.f : owner.getProgress()};
    bar.scaleToSize({bsize.x * size.x, bsize.y * size.y});

    // position based on fill dir
    switch (owner.getFillDirection()) {
    case ProgressBar::RightToLeft:
        bar.getTransform().setPosition({bsize.x - size.x, 0.f});
        break;

    case ProgressBar::BottomToTop:
        bar.getTransform().setPosition({0.f, bsize.y - size.y});
        break;

    case ProgressBar::TopToBottom:
    case ProgressBar::LeftToRight:
    default:
        bar.getTransform().setPosition({0.f, 0.f});
        break;
    }
}

void ProgressBarComponent::onRenderSettingChange() {
    const RenderSettings& settings = getOwnerAs<ProgressBar>().getRenderSettings();
    background.setFillColor(bl::sfcol(settings.fillColor.value_or(sf::Color(120, 120, 120))));
    background.setOutlineColor(bl::sfcol(settings.outlineColor.value_or(sf::Color(20, 20, 20))));
    background.setOutlineThickness(settings.outlineThickness.value_or(1.f));
    bar.setFillColor(bl::sfcol(settings.secondaryFillColor.value_or(sf::Color(114, 219, 72))));
    bar.setOutlineColor(bl::sfcol(settings.secondaryOutlineColor.value_or(sf::Color::Transparent)));
    bar.setOutlineThickness(-settings.secondaryOutlineThickness.value_or(0.f));
}

ecs::Entity ProgressBarComponent::getEntity() const { return background.entity(); }

void ProgressBarComponent::doCreate(engine::Engine& engine, rdr::Renderer&, Component*,
                                    Component&) {
    ProgressBar& owner = getOwnerAs<ProgressBar>();
    background.create(engine, {owner.getAcquisition().width, owner.getAcquisition().height});
    bar.create(engine, {100.f, 100.f});
    bar.setParent(background);
}

void ProgressBarComponent::doSceneAdd(rc::Overlay* overlay) {
    background.addToScene(overlay, rc::UpdateSpeed::Static);
    bar.addToScene(overlay, rc::UpdateSpeed::Static);
}

void ProgressBarComponent::doSceneRemove() {
    background.removeFromScene();
    bar.removeFromScene();
}

void ProgressBarComponent::handleAcquisition(const sf::Vector2f& posFromParent, const sf::Vector2f&,
                                             const sf::Vector2f& size) {
    const RenderSettings& settings = getOwnerAs<ProgressBar>().getRenderSettings();
    const float ot                 = settings.outlineThickness.value_or(1.f);
    background.setSize({size.x - ot * 2.f, size.y - ot * 2.f});
    background.getTransform().setPosition({posFromParent.x + ot, posFromParent.y + ot});
}

void ProgressBarComponent::handleMove(const sf::Vector2f& posFromParent, const sf::Vector2f&) {
    const RenderSettings& settings = getOwnerAs<ProgressBar>().getRenderSettings();
    const float ot                 = settings.outlineThickness.value_or(1.f);
    background.getTransform().setPosition({posFromParent.x + ot, posFromParent.y + ot});
}

} // namespace defcoms
} // namespace gui
} // namespace bl
