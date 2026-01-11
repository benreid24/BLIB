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

void MouseDriver::process(const sf::Event& event) {
    if (event.is<sf::Event::MouseMoved>()) {
        const sf::Event::MouseMoved& moveEvent = *event.getIf<sf::Event::MouseMoved>();
        menu.processEvent(Event(Event::LocationEvent(sf::Vector2f(moveEvent.position))));
    }
    else if (event.is<sf::Event::MouseButtonPressed>() && clickActivates) {
        const sf::Event::MouseButtonPressed& buttonEvent =
            *event.getIf<sf::Event::MouseButtonPressed>();
        if (buttonEvent.button == activateButton) {
            menu.processEvent(Event(Event::ActivateEvent()));
        }
    }
}

} // namespace menu
} // namespace bl
