#include <BLIB/GUI.hpp>

namespace bl
{
GUI::Ptr GUI::create(gui::Packer::Ptr packer, const sf::IntRect& region,
                     const std::string& group, const std::string& id) {
    return Ptr(new GUI(packer, region, group, id));
}

GUI::Ptr GUI::create(gui::Packer::Ptr packer, const sf::RenderWindow& window,
                     const std::string& group, const std::string& id) {
    return Ptr(new GUI(packer, window, group, id));
}

GUI::GUI(gui::Packer::Ptr packer, const std::string& group, const std::string& id)
: Box(packer, group, id)
, renderer(gui::Renderer::create()) {}

GUI::GUI(gui::Packer::Ptr packer, const sf::IntRect& region, const std::string& group,
         const std::string& id)
: GUI(packer, group, id) {
    assignAcquisition(region);
}

GUI::GUI(gui::Packer::Ptr packer, const sf::RenderWindow& window, const std::string& group,
         const std::string& id)
: GUI(packer, group, id) {
    assignAcquisition(sf::IntRect(0, 0, window.getSize().x, window.getSize().y));
}

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
    Container::render(target, states, renderer);
}

} // namespace bl