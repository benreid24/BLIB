#include <BLIB/Interfaces/GUI.hpp>

#include <BLIB/Engine.hpp>

namespace bl
{
namespace gui
{
GUI::Ptr GUI::create(engine::World& world, engine::Player& player, const gui::Packer::Ptr& packer,
                     const sf::FloatRect& region, rdr::FactoryTable* factory) {
    return Ptr(new GUI(world, player, packer, region, factory));
}

GUI::GUI(engine::World& world, engine::Player& player, const gui::Packer::Ptr& packer,
         const sf::FloatRect& region, rdr::FactoryTable* factory)
: Box(packer)
, engine(world.engine())
, observer(player.getRenderObserver())
, renderer(world, *this, factory ? *factory : rdr::FactoryTable::getDefaultTable()) {
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

void GUI::process(const sf::Event& event) { processEvent(event); }

bool GUI::processEvent(const sf::Event& event) {
    if (event.type == sf::Event::MouseEntered) { return false; }
    if (event.type == sf::Event::MouseMoved) {
        mousePos.x = event.mouseMove.x;
        mousePos.y = event.mouseMove.y;
        mousePos   = observer.transformToOverlaySpace(mousePos);
    }

    const Event guiEvent = Event::fromSFML(event, sf::Vector2f(mousePos.x, mousePos.y));
    if (guiEvent.type() != Event::Unknown) { return Container::processEvent(guiEvent); }
    return false;
}

void GUI::update(float dt) {
    renderer.update(dt);
    Box::update(dt);

    for (const auto& action : queuedActions) { action(); }
    queuedActions.clear();
}

void GUI::queueAction(const Element::QueuedAction& a) { queuedActions.emplace_back(a); }

void GUI::addToOverlay(rc::Overlay* overlay, bool sub) {
    if (!overlay) { overlay = observer.getOrCreateSceneOverlay(); }
    renderer.addToOverlay(overlay);
    prepareRender(renderer);
    getComponent()->assignDepth(800.f); // most things bias negative
    assignDepths();
    if (sub) { subscribe(engine.getSignalChannel()); }
}

void GUI::removeFromScene() {
    renderer.removeFromScene();
    unsubscribe();
}

} // namespace gui
} // namespace bl
