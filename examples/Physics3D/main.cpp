#include <BLIB/Cameras.hpp>
#include <BLIB/Engine.hpp>
#include <BLIB/Game.hpp>
#include <BLIB/Objects.hpp>

namespace controls
{
enum Type : unsigned int { Movement = 0, Up = 1, Down = 2, Speed = 3, Direction = 4, COUNT = 5 };

using Ctrl = bl::input::Control::Config;

const bl::input::InputConfiguration
    Config{.controls =
               {
                   {Movement,
                    bl::input::Control::Type::Movement,
                    Ctrl{.movement = {.type = bl::input::MovementControl::Type::Triggers,
                                      .triggers =
                                          {
                                              .up    = {.type = bl::input::Trigger::Type::Key,
                                                        .key  = sf::Keyboard::Key::W},
                                              .right = {.type = bl::input::Trigger::Type::Key,
                                                        .key  = sf::Keyboard::Key::D},
                                              .down  = {.type = bl::input::Trigger::Type::Key,
                                                        .key  = sf::Keyboard::Key::S},
                                              .left  = {.type = bl::input::Trigger::Type::Key,
                                                        .key  = sf::Keyboard::Key::A},
                                          }}},
                    {}},
                   {Up,
                    bl::input::Control::Type::SingleTrigger,
                    {.trigger = {.type = bl::input::Trigger::Type::Key,
                                 .key  = sf::Keyboard::Key::Space}},
                    {}},
                   {Down,
                    bl::input::Control::Type::SingleTrigger,
                    {.trigger = {.type = bl::input::Trigger::Type::Key,
                                 .key  = sf::Keyboard::Key::LControl}},
                    {}},
                   {Speed,
                    bl::input::Control::Type::SingleTrigger,
                    {.trigger = {.type = bl::input::Trigger::Type::Key,
                                 .key  = sf::Keyboard::Key::LShift}},
                    {}},
                   {Direction,
                    bl::input::Control::Type::Directional,
                    {.directional = {.type = bl::input::DirectionalControl::Type::Mouse}},
                    {}},
               }};
} // namespace controls

class DemoState : public bl::engine::State {
public:
    DemoState()
    : State(bl::engine::StateMask::All) {}

    virtual ~DemoState() = default;

    virtual const char* name() const override { return "DemoState"; }

    virtual void activate(bl::engine::Engine& engine) override {
        auto& player = engine.getPlayer();
        auto world   = player.enterWorld<bl::engine::World3D>();
        auto scene   = world->scene();

        auto& cam = *player.getRenderObserver().setCamera<bl::cam::Camera3D>(
            glm::vec3{0.f, 3.f, -5.f}, glm::vec3{0.f, 1.f, 0.f});
        auto& controller = *cam.setController<bl::cam::c3d::FreeController>(5.f, 10.f);
    }

    virtual void deactivate(bl::engine::Engine& engine) override {
        // TODO
    }

    virtual void update(bl::engine::Engine&, float dt, float) override {
        // TODO
    }

private:
    //
};

class Physics3DExample : public bl::game::Game {
private:
    bool performEarlyStartup(int, char**) override {
        // noop
        return true;
    }

    bl::engine::Settings createStartupParameters() override {
        return bl::engine::Settings()
            .withAppName("BLIB Physics3D Demo")
            .withRenderer(bl::rc::CreationSettings().withWindowSettings(
                bl::rc::WindowSettings()
                    .withVideoMode(sf::VideoMode({1920, 1080}, 32))
                    .withStyle(sf::Style::Close | sf::Style::Titlebar | sf::Style::Resize)
                    .withTitle("Physics3D Demo")));
    }

    bool completeStartup(bl::engine::Engine& engine) override {
        engine.inputSystem().configure(controls::Config);

        return true;
    }

    bl::engine::State::Ptr createInitialEngineState() override {
        return std::make_shared<DemoState>();
    }

    void startShutdown() override {
        // noop
    }

    void completeShutdown() override {
        // noop
    }
} globalGameInstance; // this just needs to exist
