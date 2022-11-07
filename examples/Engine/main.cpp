#include "Controls.hpp"
#include "EventListener.hpp"
#include "MainState.hpp"

#include <BLIB/Engine.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

void configureInput(bl::input::InputSystem& inputSystem) {
    // register our controls
    inputSystem.setControlCount(Control::Count);
    inputSystem.configureMovementControl(Control::Movement);
    inputSystem.configureTriggerControl(Control::Example);
    inputSystem.configureTriggerControl(Control::RebindExample);
    inputSystem.configureTriggerControl(Control::RebindMovementVert);
    inputSystem.configureTriggerControl(Control::Back);
    inputSystem.configureTriggerControl(Control::Close);

    // create an actor for our user
    bl::input::Actor& user = inputSystem.addActor();

    // keyboard and mouse default config
    user.getKBMMovementUpControl(Control::Movement).triggerOnKey(sf::Keyboard::W);
    user.getKBMMovementRightControl(Control::Movement).triggerOnKey(sf::Keyboard::D);
    user.getKBMMovementDownControl(Control::Movement).triggerOnKey(sf::Keyboard::S);
    user.getKBMMovementLeftControl(Control::Movement).triggerOnKey(sf::Keyboard::A);
    user.getKBMTriggerControl(Control::Back).triggerOnKey(sf::Keyboard::BackSpace);
    user.getKBMTriggerControl(Control::Close).triggerOnKey(sf::Keyboard::Escape);
    user.getKBMTriggerControl(Control::RebindExample).triggerOnKey(sf::Keyboard::E);
    user.getKBMTriggerControl(Control::RebindMovementVert).triggerOnKey(sf::Keyboard::Q);

    // controller
    using bl::input::controllers::Xbox360;
    bl::input::Joystick& jsMv = user.getJoystickMovementControl(Control::Movement);
    jsMv.horizontalAxis       = Xbox360::LSHorizontal;
    jsMv.verticalAxis         = Xbox360::LSVertical;
    jsMv.verticalInverted     = true;
    user.getJoystickTriggerControl(Control::Back).triggerOnJoystickButton(Xbox360::B);
    user.getJoystickTriggerControl(Control::Close).triggerOnJoystickButton(Xbox360::Start);
    user.getJoystickTriggerControl(Control::RebindExample).triggerOnJoystickButton(Xbox360::A);
    user.getJoystickTriggerControl(Control::RebindMovementVert).triggerOnJoystickButton(Xbox360::X);

    // load from config file if present
    if (bl::util::FileUtil::exists("config.cfg")) {
        bl::engine::Configuration::load("config.cfg");
        inputSystem.loadFromConfig();
    }
}

int main() {
    const bl::engine::Settings engineSettings = bl::engine::Settings().withWindowParameters(
        bl::engine::Settings::WindowParameters()
            .withVideoMode(sf::VideoMode(800, 600, 32))
            .withStyle(sf::Style::Close | sf::Style::Titlebar | sf::Style::Resize)
            .withTitle("BLIB Engine Example")
            .withLetterBoxOnResize(true));

    bl::engine::Engine engine(engineSettings);
    configureInput(engine.inputSystem());

    EventListener listener;
    engine.eventBus().subscribe(&listener);

    engine.run(MainState::create());

    // should call this if utilizing the Waiter utility to ensure all threads exit cleanly
    bl::util::Waiter::unblockAll();

    return 0;
}
