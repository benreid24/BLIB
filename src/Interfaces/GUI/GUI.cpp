#include <BLIB/Interfaces/GUI.hpp>

#include <BLIB/Interfaces/GUI/Renderers/DefaultRenderer.hpp>

namespace bl
{
namespace gui
{
GUI::Ptr GUI::create(const gui::Packer::Ptr& packer, const sf::FloatRect& region) {
    return Ptr(new GUI(packer, region));
}

GUI::Ptr GUI::create(const gui::Packer::Ptr& packer, const sf::RenderWindow& window) {
    return Ptr(new GUI(packer, window));
}

GUI::GUI(const gui::Packer::Ptr& packer)
: Box(packer)
, renderer(gui::DefaultRenderer::create()) {}

GUI::GUI(const gui::Packer::Ptr& packer, const sf::FloatRect& region)
: GUI(packer) {
    assignAcquisition(region);
}

GUI::GUI(const gui::Packer::Ptr& packer, const sf::RenderWindow& window)
: GUI(packer) {
    assignAcquisition(sf::FloatRect(0, 0, window.getSize().x, window.getSize().y));
}

void GUI::setRegion(const sf::FloatRect& area) { assignAcquisition(area); }

void GUI::subscribe(event::Dispatcher& d) { d.subscribe(this); }

void GUI::observe(const sf::Event& event) {
    if (event.type == sf::Event::MouseEntered) return;
    if (event.type == sf::Event::MouseMoved) {
        mousePos.x = event.mouseMove.x;
        mousePos.y = event.mouseMove.y;
    }
    sf::Transform tform = getInverseTransform();
    tform *= renderTransform.getInverse();
    const Event guiEvent = Event::fromSFML(event, tform.transformPoint(mousePos));
    if (guiEvent.type() != Event::Unknown) { Container::processEvent(guiEvent); }
}

void GUI::setRenderer(const gui::Renderer::Ptr& r) { renderer = r; }

void GUI::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    renderTransform = states.transform;
    states.transform.combine(getTransform());
    renderer->setOriginalView(target.getView());
    Container::render(target, states, *renderer);
    renderer->renderTooltip(target, states, mousePos);
}

} // namespace gui
} // namespace bl
