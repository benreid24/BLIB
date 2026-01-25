#ifndef EVENTLISTENER_HPP
#define EVENTLISTENER_HPP

#include <BLIB/Input.hpp>
#include <BLIB/Logging.hpp>
#include <BLIB/Signals.hpp>
#include <BLIB/Util/Visitor.hpp>
#include <SFML/Window.hpp>

namespace sf
{
inline std::ostream& operator<<(std::ostream& os, const Joystick::Identification& id) {
    os << "[ Name: '" << id.name.toAnsiString() << "' ProductId: " << id.productId
       << " VendorId: " << id.vendorId << " ]";
    return os;
}
} // namespace sf

class EventListener : public bl::sig::Listener<sf::Event> {
public:
    virtual ~EventListener() = default;

    virtual void process(const sf::Event& event) override {
        event.visit(bl::util::Visitor{
            [](const sf::Event::JoystickButtonPressed& ev) {
                BL_LOG_INFO << "Joystick button: " << ev.button << " from "
                            << sf::Joystick::getIdentification(ev.joystickId).name;
            },
            [](const sf::Event::JoystickMoved& ev) {
                if (std::abs(ev.position) >= 90.f) {
                    BL_LOG_INFO << "Joystick axis " << ev.axis << " -> " << ev.position << " from "
                                << sf::Joystick::getIdentification(ev.joystickId).name;
                }
            },
            [](const auto&) {
                // Do nothing
            }});
    }
};

#endif
