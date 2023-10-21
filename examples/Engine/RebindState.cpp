#include "RebindState.hpp"
#include "Controls.hpp"
#include <BLIB/Cameras.hpp>

bl::engine::State::Ptr RebindState::create(unsigned int ctrl) { return Ptr{new RebindState(ctrl)}; }

RebindState::RebindState(unsigned int ctrl)
: State(bl::engine::StateMask::All)
, configurator()
, ctrl(ctrl)
, inited(false) {
    font = bl::resource::ResourceManager<sf::VulkanFont>::load("font.ttf");
}

const char* RebindState::name() const { return "RebindState"; }

void RebindState::activate(bl::engine::Engine& engine) {
    engine.renderer().getObserver().setClearColor({1.f, 1.f, 1.f, 1.f});

    if (!inited) {
        text.create(engine, *font, "", 22, {0.1f, 0.65f, 0.22f, 1.f});
        text.getTransform().setPosition({50.f, 300.f});
    }

    if (ctrl == Control::Movement) {
        configurator.start(engine.inputSystem().getActor().getJoystickMovementControl(ctrl));
    }
    else {
        configurator.start(engine.inputSystem().getActor().getKBMTriggerControl(ctrl));
        text.getSection().setString("Press the desired control (keyboard/mouse only)");
    }

    bl::rc::SceneRef scene = engine.renderer().getObserver().pushScene<bl::rc::scene::Scene2D>();
    engine.renderer().getObserver().setCamera<bl::cam::Camera2D>(
        sf::FloatRect{0.f, 0.f, 800.f, 600.f});
    text.addToScene(scene, bl::rc::UpdateSpeed::Static);
}

void RebindState::deactivate(bl::engine::Engine& engine) {
    engine.renderer().getObserver().popScene();

    // save new input to config file
    engine.inputSystem().saveToConfig();
    bl::engine::Configuration::save("config.cfg");
}

void RebindState::update(bl::engine::Engine& engine, float, float) {
    switch (configurator.getState()) {
    case bl::input::Configurator::WaitingVertAxis:
        text.getSection().setString("Move the desired up/down stick back and forth a few times");
        break;
    case bl::input::Configurator::WaitingHorAxis:
        text.getSection().setString("Move the desired left/right stick back and forth a few times");
        break;
    case bl::input::Configurator::Finished:
        engine.popState();
        break;
    default:
        break;
    }
}
