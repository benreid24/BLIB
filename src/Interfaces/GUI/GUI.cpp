#include <BLIB/Interfaces/GUI.hpp>

#include <BLIB/Interfaces/GUI/Renderers/DefaultRenderer.hpp>

namespace bl
{
namespace gui
{
GUI::Ptr GUI::create(gui::Packer::Ptr packer, const sf::IntRect& region) {
    return Ptr(new GUI(packer, region));
}

GUI::Ptr GUI::create(gui::Packer::Ptr packer, const sf::RenderWindow& window) {
    return Ptr(new GUI(packer, window));
}

GUI::GUI(gui::Packer::Ptr packer)
: Box(packer)
, renderer(gui::DefaultRenderer::create()) {}

GUI::GUI(gui::Packer::Ptr packer, const sf::IntRect& region)
: GUI(packer) {
    assignAcquisition(region);
}

GUI::GUI(gui::Packer::Ptr packer, const sf::RenderWindow& window)
: GUI(packer) {
    assignAcquisition(sf::IntRect(0, 0, window.getSize().x, window.getSize().y));
}

void GUI::setRegion(const sf::IntRect& area) { assignAcquisition(area); }

void GUI::subscribe(event::Dispatcher& d) { d.subscribe(this); }

void GUI::observe(const sf::Event& event) {
    if (event.type == sf::Event::MouseMoved) {
        mousePos.x = event.mouseMove.x;
        mousePos.y = event.mouseMove.y;
    }
    sf::Transform tform = getInverseTransform();
    tform *= renderTransform.getInverse();
    Container::handleEvent(gui::RawEvent(event, mousePos, tform));
}

void GUI::setRenderer(gui::Renderer::Ptr r) { renderer = r; }

void GUI::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    renderTransform = states.transform;
    states.transform.combine(getTransform());
    Container::render(target, states, *renderer);
}

} // namespace gui
} // namespace bl
