#include "RebindState.hpp"
#include "Controls.hpp"

bl::engine::State::Ptr RebindState::create(bl::engine::Engine& engine, unsigned int ctrl) {
    return Ptr{new RebindState(engine, ctrl)};
}

RebindState::RebindState(bl::engine::Engine& engine, unsigned int ctrl)
: configurator(engine.eventBus()) {
    font = bl::engine::Resources::fonts().load("font.ttf").data;
    text.setFont(*font);
    text.setPosition(50.f, 300.f);
    text.setFillColor(sf::Color(20, 170, 55));

    if (ctrl == Control::Movement) {
        configurator.start(
            engine.inputSystem().getActor().getJoystickMovementControl(ctrl).joystickControl());
    }
    else {
        configurator.start(engine.inputSystem().getActor().getKBMTriggerControl(ctrl));
        text.setString("Press the desired control (keyboard/mouse only)");
    }
}

const char* RebindState::name() const { return "RebindState"; }

void RebindState::activate(bl::engine::Engine&) {
    // nothing here
}

void RebindState::deactivate(bl::engine::Engine& engine) {
    // save new input to config file
    engine.inputSystem().saveToConfig();
    bl::engine::Configuration::save("config.cfg");
}

void RebindState::update(bl::engine::Engine& engine, float) {
    switch (configurator.getState()) {
    case bl::input::Configurator::WaitingVertAxis:
        text.setString("Move the desired up/down stick back and forth a few times");
        break;
    case bl::input::Configurator::WaitingHorAxis:
        text.setString("Move the desired left/right stick back and forth a few times");
        break;
    case bl::input::Configurator::Finished:
        engine.popState();
        break;
    default:
        break;
    }
}

void RebindState::render(bl::engine::Engine& engine, float) {
    engine.window().clear(sf::Color::White);
    engine.window().draw(text);
    engine.window().display();
}
