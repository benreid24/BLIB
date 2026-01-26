#include "MainState.hpp"
#include "Controls.hpp"
#include "RebindState.hpp"
#include <BLIB/Cameras.hpp>
#include <BLIB/Resources.hpp>

bl::engine::State::Ptr MainState::create() { return Ptr{new MainState()}; }

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
    user.getKBMMovementUpControl(Control::Movement).triggerOnKey(sf::Keyboard::Key::W);
    user.getKBMMovementRightControl(Control::Movement).triggerOnKey(sf::Keyboard::Key::D);
    user.getKBMMovementDownControl(Control::Movement).triggerOnKey(sf::Keyboard::Key::S);
    user.getKBMMovementLeftControl(Control::Movement).triggerOnKey(sf::Keyboard::Key::A);
    user.getKBMTriggerControl(Control::Back).triggerOnKey(sf::Keyboard::Key::Backspace);
    user.getKBMTriggerControl(Control::Close).triggerOnKey(sf::Keyboard::Key::Escape);
    user.getKBMTriggerControl(Control::RebindExample).triggerOnKey(sf::Keyboard::Key::E);
    user.getKBMTriggerControl(Control::RebindMovementVert).triggerOnKey(sf::Keyboard::Key::Q);

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

MainState::MainState()
: State(bl::engine::StateMask::All)
, inited(false) {}

const char* MainState::name() const { return "MainState"; }

void MainState::activate(bl::engine::Engine& engine) {
    configureInput(engine);
    engine.renderer().setClearColor({1.f, 0.f, 0.f});
    engine.renderer().getObserver().setClearColor({0.f, 1.f, 0.f, 1.f});

    if (!inited) {
        inited = true;

        world = engine.createWorld<bl::engine::BasicWorld<bl::rc::scene::Scene2D>>();

        font = bl::resource::ResourceManager<sf::VulkanFont>::load("font.ttf");
        listener.init(*world);

        kbmControls.create(*world, *font, "", 22, {0.f, 0.f, 0.f, 1.f});
        kbmControls.getTransform().setPosition({400.f, 5.f});

        jsControls.create(*world, *font, "", 22, {0.f, 0.f, 0.f, 1.f});
        jsControls.getTransform().setPosition({400.f, 355.f});

        cover.create(*world, {800.f, 800.f});
        cover.setFillColor({1.f, 1.f, 1.f, 1.f});
    }

    bl::rc::SceneRef scene = world->scene();
    engine.getPlayer().enterWorld(world);
    kbmControls.addToScene(scene, bl::rc::UpdateSpeed::Static);
    jsControls.addToScene(scene, bl::rc::UpdateSpeed::Static);
    cover.addToScene(scene, bl::rc::UpdateSpeed::Static);
    listener.addToScene(scene);

    engine.renderer().getObserver().setCamera<bl::cam::Camera2D>(
        sf::FloatRect{{0.f, 0.f}, {800.f, 600.f}});
    engine.inputSystem().getActor().addListener(listener);
}

void MainState::deactivate(bl::engine::Engine& engine) {
    engine.getPlayer().leaveWorld();
    engine.inputSystem().getActor().removeListener(listener);
}

void MainState::update(bl::engine::Engine& engine, float, float) {
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

    if (listener.shouldRebindExample()) { engine.pushState(RebindState::create(Control::Example)); }
    else if (listener.shouldRebindMovement()) {
        engine.pushState(RebindState::create(Control::Movement));
    }
}
