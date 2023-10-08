#include "MainState.hpp"
#include "Controls.hpp"
#include "RebindState.hpp"
#include <BLIB/Cameras.hpp>
#include <BLIB/Resources.hpp>

bl::engine::State::Ptr MainState::create() { return Ptr{new MainState()}; }

MainState::MainState()
: State(bl::engine::StateMask::All)
, inited(false) {}

const char* MainState::name() const { return "MainState"; }

void MainState::activate(bl::engine::Engine& engine) {
    engine.renderer().setClearColor({0.f, 0.f, 0.f});

    if (!inited) {
        inited = true;

        font = bl::resource::ResourceManager<sf::VulkanFont>::load("font.ttf");
        listener.init(engine);

        kbmControls.create(engine, *font, "", 22, {0.f, 0.f, 0.f, 1.f});
        kbmControls.getTransform().setPosition({400.f, 5.f});

        jsControls.create(engine, *font, "", 22, {0.f, 0.f, 0.f, 1.f});
        jsControls.getTransform().setPosition({400.f, 355.f});

        cover.create(engine, {800.f, 800.f});
        cover.setFillColor({1.f, 1.f, 1.f, 1.f});
    }

    bl::rc::Scene* scene = engine.renderer().getObserver().pushScene<bl::rc::scene::BatchedScene>();
    kbmControls.addToScene(scene, bl::rc::UpdateSpeed::Static);
    jsControls.addToScene(scene, bl::rc::UpdateSpeed::Static);
    cover.addToScene(scene, bl::rc::UpdateSpeed::Static);
    listener.addToScene(scene);

    engine.renderer().getObserver().setCamera<bl::cam::Camera2D>(
        sf::FloatRect{0.f, 0.f, 800.f, 600.f});
    engine.inputSystem().getActor().addListener(listener);
}

void MainState::deactivate(bl::engine::Engine& engine) {
    engine.renderer().getObserver().popScene();
    engine.inputSystem().getActor().removeListener(listener);
}

void MainState::update(bl::engine::Engine& engine, float) {
    const bl::input::Actor& a = engine.inputSystem().getActor();
    kbmControls.getSection().setString(
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

    jsControls.getSection().setString(
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

void MainState::render(bl::engine::Engine&, float) {
    // deprecated
}
