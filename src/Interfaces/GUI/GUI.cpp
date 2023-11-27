#include <BLIB/Interfaces/GUI.hpp>

namespace bl
{
namespace gui
{
GUI::Ptr GUI::create(engine::Engine& engine, rc::Observer& observer, const gui::Packer::Ptr& packer,
                     const sf::FloatRect& region, rdr::FactoryTable* factory) {
    return Ptr(new GUI(engine, observer, packer, region, factory));
}

GUI::GUI(engine::Engine& engine, rc::Observer& observer, const gui::Packer::Ptr& packer,
         const sf::FloatRect& region, rdr::FactoryTable* factory)
: Box(packer)
, observer(observer)
, renderer(engine, *this, factory ? *factory : rdr::FactoryTable::getDefaultTable()) {
    setConstrainView(false);
    setOutlineThickness(0.f);
    queuedActions.reserve(4);
    assignAcquisition(region.width > 0.f ?
                          region :
                          sf::FloatRect(0.f,
                                        0.f,
                                        cam::OverlayCamera::getOverlayCoordinateSpace().x,
                                        cam::OverlayCamera::getOverlayCoordinateSpace().y));
}

void GUI::setRegion(const sf::FloatRect& area) { assignAcquisition(area); }

void GUI::observe(const sf::Event& event) {
    if (event.type == sf::Event::MouseEntered) return;
    if (event.type == sf::Event::MouseMoved) {
        mousePos.x = event.mouseMove.x;
        mousePos.y = event.mouseMove.y;
        mousePos   = observer.transformToOverlaySpace(mousePos);
    }

    const Event guiEvent = Event::fromSFML(event, sf::Vector2f(mousePos.x, mousePos.y));
    if (guiEvent.type() != Event::Unknown) { Container::processEvent(guiEvent); }
}

void GUI::update(float dt) {
    renderer.update(dt);
    Box::update(dt);

    for (const auto& action : queuedActions) { action(); }
    queuedActions.clear();
}

void GUI::queueAction(const Element::QueuedAction& a) { queuedActions.emplace_back(a); }

void GUI::addToOverlay(rc::Overlay* overlay) {
    if (!overlay) { overlay = observer.getOrCreateSceneOverlay(); }
    renderer.addToOverlay(overlay);
    prepareRender(renderer);
    getComponent()->assignDepth(800.f); // most things bias negative
    assignDepths();
    bl::event::Dispatcher::subscribe(this);
}

void GUI::removeFromOverlay() {
    renderer.removeFromOverlay();
    bl::event::Dispatcher::unsubscribe(this);
}

} // namespace gui
} // namespace bl
