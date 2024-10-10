#include "InputListener.hpp"
#include "Controls.hpp"
#include <BLIB/Input.hpp>
#include <BLIB/Logging.hpp>

InputListener::InputListener()
: rebindExample(false)
, rebindMovement(false) {}

void InputListener::init(bl::engine::World& world) {
    font = bl::resource::ResourceManager<sf::VulkanFont>::load("font.ttf");
    text.create(world, *font, "Last input:", 22, {0.f, 0.2f, 0.72f, 1.f});
    text.getTransform().setPosition({10.f, 50.f});
}

void InputListener::addToScene(bl::rc::Scene* scene) {
    text.addToScene(scene, bl::rc::UpdateSpeed::Static);
}

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
    text.getSection().setString("Last input:\n  " + ctrl);
    return true;
}
