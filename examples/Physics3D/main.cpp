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

const bl::cam::c3d::FreeController::ControlScheme CameraControls{.movement      = Movement,
                                                                 .up            = Up,
                                                                 .down          = Down,
                                                                 .speed         = Speed,
                                                                 .speedFactor   = 3.f,
                                                                 .direction     = Direction,
                                                                 .rotationSpeed = 1.5f};

} // namespace controls

namespace
{
void colorVertex(bl::rc::prim::Vertex3D& v) {
    v.color = glm::vec4{v.pos.x / 300.f + 0.5f, v.pos.y / 30.f, v.pos.z / 300.f + 0.5f, 1.f};
}
} // namespace

class DemoState
: public bl::engine::State
, bl::sig::Listener<sf::Event> {
public:
    DemoState()
    : State(bl::engine::StateMask::All) {}

    virtual ~DemoState() = default;

    virtual const char* name() const override { return "DemoState"; }

    virtual void activate(bl::engine::Engine& engine) override {
        auto& player = engine.getPlayer();
        auto world   = player.enterWorld<bl::engine::World3D>();
        auto scene   = world->scene();
        inputSystem  = &engine.inputSystem();

        auto& cam = *player.getRenderObserver().setCamera<bl::cam::Camera3D>(
            glm::vec3{0.f, 35.f, -50.f}, glm::vec3{0.f, 1.f, 0.f});
        auto& controller = *cam.setController<bl::cam::c3d::FreeController>(10.f, 15.f);
        controller.setControlScheme(controls::CameraControls);
        controller.subscribeToInput(player);

        bl::rand::Perlin<float> perlin(1234);
        terrain.createFromNoise2d(*world, perlin, 300.f, 300.f, 30.f, 1.f, 6, 0.5f, 0.01f);
        terrain.addToScene(scene, bl::rc::UpdateSpeed::Static);
        terrain.createPhysicsBody(JPH::EMotionType::Static,
                                  bl::com::Physics3D::ObjectLayer::Terrain);
        for (auto& v : terrain.component().gpuBuffer.vertices()) { colorVertex(v); }

        engine.inputSystem().captureMouse();
        subscribe(engine.getSignalChannel());
    }

    virtual void deactivate(bl::engine::Engine& engine) override {
        unsubscribe();
        engine.getPlayer().leaveWorld();
    }

    virtual void update(bl::engine::Engine&, float, float) override {
        // noop
    }

    virtual void process(const sf::Event& event) override {
        const sf::Event::KeyPressed* keyEvent = event.getIf<sf::Event::KeyPressed>();
        if (keyEvent && keyEvent->code == sf::Keyboard::Key::Escape) { release(); }

        const sf::Event::FocusGained* focusEvent = event.getIf<sf::Event::FocusGained>();
        if (focusEvent) { capture(); }

        const sf::Event::MouseButtonPressed* mouseEvent =
            event.getIf<sf::Event::MouseButtonPressed>();
        if (mouseEvent && mouseEvent->button == sf::Mouse::Button::Left) { capture(); }
    }

private:
    bl::input::InputSystem* inputSystem;
    bl::input::ListenerBoundary listenerBoundary;
    bl::obj::o3d::HeightmapTerrainBasic terrain;

    void capture() {
        inputSystem->getActor().removeListener(listenerBoundary);
        inputSystem->captureMouse();
    }

    void release() {
        inputSystem->getActor().addListener(listenerBoundary);
        inputSystem->releaseMouse();
    }
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
