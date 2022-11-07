#include <BLIB/Interfaces/Menu/Drivers/MouseDriver.hpp>

namespace bl
{
namespace menu
{
MouseDriver::MouseDriver(Menu& menu, bool allowClick, sf::Mouse::Button abutt)
: menu(menu)
, clickActivates(allowClick)
, activateButton(abutt) {}

void MouseDriver::setAllowButtonActivate(bool allow) { clickActivates = allow; }

void MouseDriver::setMouseActivateButton(sf::Mouse::Button butt) { activateButton = butt; }

void MouseDriver::observe(const sf::Event& event) {
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
