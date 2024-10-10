#include "Controls.hpp"
#include "EventListener.hpp"
#include "MainState.hpp"

#include <BLIB/Engine.hpp>
#include <BLIB/Game.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

void configureInput(bl::engine::Engine& engine) {
    bl::input::InputSystem& inputSystem = engine.inputSystem();
    bl::input::Actor& user              = engine.getPlayer().getInputActor();

    // register our controls
    inputSystem.setControlCount(Control::Count);
    inputSystem.configureMovementControl(Control::Movement);
    inputSystem.configureTriggerControl(Control::Example);
    inputSystem.configureTriggerControl(Control::RebindExample);
    inputSystem.configureTriggerControl(Control::RebindMovementVert);
    inputSystem.configureTriggerControl(Control::Back);
    inputSystem.configureTriggerControl(Control::Close);

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

class EngineExample : public bl::game::Game {
    EventListener listener;

    bool performEarlyStartup(int, char**) override {
        bl::event::Dispatcher::subscribe(&listener);
        return true;
    }

    bl::engine::Settings createStartupParameters() override {
        return bl::engine::Settings().withWindowParameters(
            bl::engine::Settings::WindowParameters()
                .withVideoMode(sf::VideoMode(800, 600, 32))
                .withStyle(sf::Style::Close | sf::Style::Titlebar | sf::Style::Resize)
                .withTitle("BLIB Engine Example")
                .withLetterBoxOnResize(true));
    }

    bool completeStartup(bl::engine::Engine& engine) override {
        configureInput(engine);
        return true;
    }

    bl::engine::State::Ptr createInitialEngineState() override { return MainState::create(); }

    void startShutdown() override { bl::event::Dispatcher::unsubscribe(&listener); }

    void completeShutdown() override {
        // noop
    }
} globalGameInstance; // this just needs to exist
