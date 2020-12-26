#include <BLIB/Menu/EventGenerators/MouseEventGenerator.hpp>

namespace bl
{
namespace menu
{
MouseEventGenerator::MouseEventGenerator(Menu& menu, bool allowClick, sf::Mouse::Button abutt)
: menu(menu)
, clickActivates(allowClick)
, activateButton(abutt) {}

void MouseEventGenerator::setAllowButtonActivate(bool allow) { clickActivates = allow; }

void MouseEventGenerator::setMouseActivateButton(sf::Mouse::Button butt) {
    activateButton = butt;
}

void MouseEventGenerator::observe(const sf::Event& event) {
    if (event.type == sf::Event::MouseMoved) {
        menu.processEvent(Event(Event::LocationEvent(
            {static_cast<float>(event.mouseMove.x), static_cast<float>(event.mouseMove.y)})));
    }
    else if (event.type == sf::Event::MouseButtonPressed && clickActivates) {
        if (event.mouseButton.button == activateButton) {
            menu.processEvent(Event(Event::ActivateEvent()));
        }
    }
}

} // namespace menu
} // namespace bl
