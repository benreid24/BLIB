#ifndef EVENTLISTENER_HPP
#define EVENTLISTENER_HPP

#include <BLIB/Events.hpp>
#include <BLIB/Input.hpp>
#include <BLIB/Logging.hpp>
#include <SFML/Window.hpp>

std::ostream& operator<<(std::ostream& os, const sf::Joystick::Identification& id) {
    os << "[ Name: '" << id.name.toAnsiString() << "' ProductId: " << id.productId
       << " VendorId: " << id.vendorId << " ]";
    return os;
}

class EventListener : public bl::event::Listener<sf::Event> {
public:
    virtual ~EventListener() = default;

    virtual void observe(const sf::Event& event) override {
        switch (event.type) {
        case sf::Event::JoystickButtonPressed:
            BL_LOG_INFO << "Joystick button: " << event.joystickButton.button << " from "
                        << sf::Joystick::getIdentification(event.joystickButton.joystickId);
            break;
        case sf::Event::JoystickMoved:
            if (std::abs(event.joystickMove.position) >= 90.f) {
                BL_LOG_INFO << "Joystick axis " << event.joystickMove.axis << " -> "
                            << event.joystickMove.position << " from "
                            << sf::Joystick::getIdentification(event.joystickMove.joystickId);
            }
            break;
        default:
            break;
        }
    }
};

#endif
