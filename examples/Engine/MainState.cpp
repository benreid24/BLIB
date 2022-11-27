#include "MainState.hpp"
#include "Controls.hpp"
#include "RebindState.hpp"
#include <BLIB/Resources.hpp>

bl::engine::State::Ptr MainState::create() { return Ptr{new MainState()}; }

MainState::MainState() {
    font = bl::resource::ResourceManager<sf::Font>::load("font.ttf").data;

    kbmControls.setFont(*font);
    kbmControls.setFillColor(sf::Color::Black);
    kbmControls.setCharacterSize(22);
    kbmControls.setPosition(400.f, 5.f);

    jsControls.setFont(*font);
    jsControls.setFillColor(sf::Color::Black);
    jsControls.setCharacterSize(22);
    jsControls.setPosition(400.f, 355.f);

    cover.setSize({800.f, 800.f});
    cover.setFillColor(sf::Color::White);
}

const char* MainState::name() const { return "MainState"; }

void MainState::activate(bl::engine::Engine& engine) {
    engine.renderSystem().cameras().pushCamera(
        bl::render::camera::StaticCamera::create({800.f, 600.f}));
    engine.inputSystem().getActor().addListener(listener);
}

void MainState::deactivate(bl::engine::Engine& engine) {
    engine.renderSystem().cameras().popCamera();
    engine.inputSystem().getActor().removeListener(listener);
}

void MainState::update(bl::engine::Engine& engine, float) {
    const bl::input::Actor& a = engine.inputSystem().getActor();
    kbmControls.setString(
        std::string("Keyboard Controls:") +
        "\n  Move up: " + a.getKBMMovementUpControl(Control::Movement).toString() +
        "\n  Move right: " + a.getKBMMovementRightControl(Control::Movement).toString() +
        "\n  Move down: " + a.getKBMMovementDownControl(Control::Movement).toString() +
        "\n  Move left: " + a.getKBMMovementLeftControl(Control::Movement).toString() +
        "\n  Example: " + a.getKBMTriggerControl(Control::Example).toString() +
        "\n  Rebind Example: " + a.getKBMTriggerControl(Control::RebindExample).toString() +
        "\n  Rebind Movement: " + a.getKBMTriggerControl(Control::RebindMovementVert).toString() +
        "\n  Back: " + a.getKBMTriggerControl(Control::Back).toString() +
        "\n  Close: " + a.getKBMTriggerControl(Control::Close).toString());

    jsControls.setString(
        "Keyboard Controls:\n  Movement: " +
        a.getJoystickMovementControl(Control::Movement).toString() +
        "\n  Example: " + a.getJoystickTriggerControl(Control::Example).toString() +
        "\n  Rebind Example: " + a.getJoystickTriggerControl(Control::RebindExample).toString() +
        "\n  Rebind Movement: " +
        a.getJoystickTriggerControl(Control::RebindMovementVert).toString() +
        "\n  Back: " + a.getJoystickTriggerControl(Control::Back).toString() +
        "\n  Close: " + a.getJoystickTriggerControl(Control::Close).toString());

    if (listener.shouldRebindExample()) {
        engine.pushState(RebindState::create(engine, Control::Example));
    }
    else if (listener.shouldRebindMovement()) {
        engine.pushState(RebindState::create(engine, Control::Movement));
    }
}

void MainState::render(bl::engine::Engine& engine, float) {
    engine.window().clear(sf::Color::Black);
    engine.window().draw(cover);
    engine.window().draw(kbmControls);
    engine.window().draw(jsControls);
    listener.render(engine.window());
    engine.window().display();
}
