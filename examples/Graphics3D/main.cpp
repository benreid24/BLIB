#include <BLIB/Cameras.hpp>
#include <BLIB/Components.hpp>
#include <BLIB/Engine.hpp>
#include <BLIB/Graphics.hpp>
#include <BLIB/Render.hpp>
#include <BLIB/Systems.hpp>
#include <iostream>

class CameraController
: public bl::cam::CameraController3D
, public bl::event::Listener<sf::Event> {
public:
    static constexpr float DistancePercentPerTick = 0.05f;
    static constexpr float YawPerPixel            = bl::math::Pi / 1920.f;
    static constexpr float PitchPerPixel          = bl::math::Pi / 1080.f;

    CameraController()
    : distance(5.f)
    , yaw(0.f)
    , pitch(bl::math::Pi / 4.f)
    , dragging(false) {}

    virtual ~CameraController() = default;

    virtual void update(float dt) override {
        const float cosPitch = std::cos(pitch);
        const float x        = std::cos(yaw) * distance * cosPitch;
        const float y        = std::sin(pitch) * distance;
        const float z        = std::sin(yaw) * distance * cosPitch;

        camera().setPosition({x, y, z});
        camera().getOrientationForChange().lookAt(glm::vec3(0.f), camera().getPosition());
    }

    virtual void observe(const sf::Event& event) override {
        switch (event.type) {
        case sf::Event::MouseWheelScrolled:
            distance *= 1.f - event.mouseWheelScroll.delta * DistancePercentPerTick;
            break;

        case sf::Event::MouseButtonPressed:
            if (event.mouseButton.button == sf::Mouse::Left) {
                dragging = true;
                prevDrag = glm::vec2(event.mouseButton.x, event.mouseButton.y);
            }
            break;

        case sf::Event::MouseButtonReleased:
            if (event.mouseButton.button == sf::Mouse::Left) { dragging = false; }
            break;

        case sf::Event::MouseMoved:
            if (dragging) {
                const glm::vec2 pos(event.mouseMove.x, event.mouseMove.y);
                const glm::vec2 diff = pos - prevDrag;
                prevDrag             = pos;

                yaw += diff.x * YawPerPixel;
                pitch += diff.y * PitchPerPixel;
            }
            break;
        }
    }

private:
    float distance;
    float yaw;
    float pitch;

    bool dragging;
    glm::vec2 prevDrag;
};

class DemoState : public bl::engine::State {
public:
    DemoState()
    : State(bl::engine::StateMask::All)
    , scene(nullptr) {}

    virtual ~DemoState() = default;

private:
    bl::rc::Scene* scene;
    bl::gfx::Cube cube1;
    bl::gfx::VertexBuffer3D floor;
    CameraController* controller;

    virtual const char* name() const override { return "DemoState"; }

    virtual void activate(bl::engine::Engine& engine) override {
        // create scene and camera
        auto world =
            engine.getPlayer().enterWorld<bl::engine::BasicWorld<bl::rc::scene::Scene3D>>();
        scene      = world->scene();
        auto& o    = engine.renderer().getObserver();
        auto* cam  = o.setCamera<bl::cam::Camera3D>(glm::vec3(0.f, 3.f, 5.f), glm::vec3(0.f));
        controller = cam->setController<CameraController>();
        o.setClearColor({0.f, 0.f, 0.f, 1.f});

        constexpr float FloorSize = 5.f;
        floor.create(*world, 6);
        floor[0].pos   = {-FloorSize, 0.f, -FloorSize};
        floor[0].color = {0.f, 1.f, 1.f, 1.f};
        floor[1].pos   = {FloorSize, 0.f, FloorSize};
        floor[1].color = {1.f, 0.f, 0.f, 1.f};
        floor[2].pos   = {-FloorSize, 0.f, FloorSize};
        floor[2].color = {0.f, 1.f, 0.f, 1.f};
        floor[3].pos   = {-FloorSize, 0.f, -FloorSize};
        floor[3].color = {0.f, 1.f, 1.f, 1.f};
        floor[4].pos   = {FloorSize, 0.f, -FloorSize};
        floor[4].color = {1.f, 0.f, 1.f, 1.f};
        floor[5].pos   = {FloorSize, 0.f, FloorSize};
        floor[5].color = {1.f, 0.f, 0.f, 1.f};
        floor.commit();
        floor.addToScene(scene, bl::rc::UpdateSpeed::Static);

        auto containerTexture =
            engine.renderer().texturePool().getOrLoadTexture("Resources/Textures/container.jpg");
        auto material = engine.renderer().materialPool().getOrCreateFromTexture(containerTexture);
        cube1.create(*world, 1.f, material);
        cube1.getTransform().setPosition({0.f, 0.501f, 0.f});
        cube1.addToScene(scene, bl::rc::UpdateSpeed::Static);

        bl::event::Dispatcher::subscribe(controller);
    }

    virtual void deactivate(bl::engine::Engine& engine) override {
        bl::event::Dispatcher::unsubscribe(controller);
        engine.getPlayer().leaveWorld();
    }

    virtual void update(bl::engine::Engine&, float, float) override {
        // noop
    }
};

int main() {
    bl::logging::Config::configureOutput(std::cout, bl::logging::Config::Debug);
    const bl::engine::Settings engineSettings = bl::engine::Settings().withWindowParameters(
        bl::engine::Settings::WindowParameters()
            .withVideoMode(sf::VideoMode(1920, 1080, 32))
            .withStyle(sf::Style::Close | sf::Style::Titlebar | sf::Style::Resize)
            .withTitle("3D Graphics Demo")
            .withLetterBoxOnResize(false));
    bl::engine::Engine engine(engineSettings);

    engine.run(std::make_shared<DemoState>());

    return 0;
}
