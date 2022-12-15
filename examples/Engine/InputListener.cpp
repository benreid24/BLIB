#include "InputListener.hpp"
#include "Controls.hpp"
#include <BLIB/Input.hpp>
#include <BLIB/Logging.hpp>

InputListener::InputListener()
: rebindExample(false)
, rebindMovement(false) {
    font = bl::resource::ResourceManager<sf::Font>::load("font.ttf");
    text.setFont(*font);
    text.setFillColor(sf::Color(0, 50, 185));
    text.setPosition(10.f, 50.f);
    text.setString("Last input:");
}

void InputListener::render(sf::RenderTarget& target) { target.draw(text); }

bool InputListener::shouldRebindExample() {
    const bool r  = rebindExample;
    rebindExample = false;
    return r;
}

bool InputListener::shouldRebindMovement() {
    const bool r   = rebindMovement;
    rebindMovement = false;
    return r;
}

bool InputListener::observe(const bl::input::Actor& actor, unsigned int activatedControl,
                            bl::input::DispatchType eventType, bool fromEvent) {
    BL_LOG_DEBUG << "Received control: " << activatedControl;

    std::string ctrl;
    switch (activatedControl) {
    case Control::Movement:
        switch (eventType) {
        case bl::input::DispatchType::MovementUp:
            ctrl = "Move Up";
            break;
        case bl::input::DispatchType::MovementRight:
            ctrl = "Move Right";
            break;
        case bl::input::DispatchType::MovementDown:
            ctrl = "Move Down";
            break;
        case bl::input::DispatchType::MovementLeft:
            ctrl = "Move Left";
            break;
        default:
            ctrl = "Unknown movement";
            break;
        }
        break;
    case Control::Example:
        ctrl = "Example";
        break;
    case Control::RebindExample:
        ctrl          = "Rebind Example";
        rebindExample = true;
        break;
    case Control::RebindMovementVert:
        ctrl           = "Rebind Movement";
        rebindMovement = true;
        break;
    case Control::Back:
        ctrl = "Back";
        break;
    case Control::Close:
        ctrl = "Close";
        break;
    default:
        ctrl = "Unknown!";
        break;
    }
    ctrl += actor.joystickMode() ? "\n(Controller)" : "\n(Keyboard/Mouse)";
    ctrl += fromEvent ? "\n(User event)" : "\n(Repeated input)";
    text.setString("Last input:\n  " + ctrl);
    return true;
}
