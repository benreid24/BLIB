#include <BLIB/Cameras/2D/Camera2D.hpp>
#include <BLIB/Engine.hpp>
#include <BLIB/Game.hpp>
#include <BLIB/Graphics.hpp>
#include <BLIB/Systems/Physics2D.hpp>

class DemoState
: public bl::engine::State
, public bl::sig::Listener<bl::sys::Physics2D::EntityCollisionBeginEvent,
                           bl::sys::Physics2D::EntityCollisionEndEvent> {
public:
    DemoState()
    : State(bl::engine::StateMask::All) {}

    virtual const char* name() const override { return "DemoState"; }

    virtual void activate(bl::engine::Engine& engine) override {
        auto world = engine.getPlayer().enterWorld<bl::engine::World2D>();
        auto scene = world->scene();
        engine.renderer().getObserver().setCamera<bl::cam::Camera2D>(glm::vec2{400.f, 300.f},
                                                                     glm::vec2{800.f, 600.f});
        engine.renderer().getObserver().setClearColor({0.f, 0.4f, 1.f, 1.f});

        floor.create(*world, {600.f, 100.f});
        floor.setFillColor(sf::Color(20, 245, 120));
        floor.getTransform().setPosition(100.f, 500.f);
        floor.addToScene(scene, bl::rc::UpdateSpeed::Static);

        playerBox.create(*world, {50.f, 50.f});
        playerBox.setFillColor(sf::Color::Red);
        playerBox.getTransform().setPosition(400.f, 200.f);
        playerBox.getTransform().setOrigin(25.f, 25.f);
        playerBox.addToScene(scene, bl::rc::UpdateSpeed::Dynamic);

        // setup physics scale
        auto& physics = engine.systems().getSystem<bl::sys::Physics2D>();
        world->setLengthUnitScale(1.f / 100.f);
        world->setGravity({0.f, 10.f});

        auto bodyDef  = b2DefaultBodyDef();
        auto shapeDef = b2DefaultShapeDef();

        // add physics for our floor
        bodyDef.type      = b2_staticBody;
        shapeDef.friction = 0.45f;
        engine.ecs().emplaceComponent<bl::com::Hitbox2D>(
            floor.entity(), &floor.getTransform(), floor.getSize());
        physics.addPhysicsToEntity(floor.entity(), bodyDef, shapeDef);

        // add physics to the player box
        bodyDef.type      = b2_dynamicBody;
        shapeDef.friction = 1.f;
        engine.ecs().emplaceComponent<bl::com::Hitbox2D>(
            playerBox.entity(), &playerBox.getTransform(), playerBox.getSize());
        physics.addPhysicsToEntity(playerBox.entity(), bodyDef, shapeDef);
        playerPhysics = engine.ecs().getComponent<bl::com::Physics2D>(playerBox.entity());

        // create a ball
        ball.create(*world, 20.f);
        ball.setFillColor(sf::Color::Blue);
        ball.getTransform().setPosition(250.f, 200.f);
        ball.getTransform().setOrigin(20.f, 20.f);
        ball.addToScene(scene, bl::rc::UpdateSpeed::Dynamic);

        // add physics to it
        engine.ecs().emplaceComponent<bl::com::Hitbox2D>(ball.entity(), &ball.getTransform(), 20.f);
        physics.addPhysicsToEntity(ball.entity(), bodyDef, shapeDef);

        onGround         = false;
        teleportCooldown = 0.f;
        subscribe(physics.getSignalChannel());
    }

    virtual void deactivate(bl::engine::Engine& engine) override {
        unsubscribe();
        engine.getPlayer().leaveWorld();
        floor.destroy();
        playerBox.destroy();
    }

    virtual void update(bl::engine::Engine&, float dt, float) override {
        teleportCooldown += dt;

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)) {
            playerPhysics->applyForceToCenter({-5.f, 0.f});
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) {
            playerPhysics->applyForceToCenter({5.f, 0.f});
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W)) {
            playerPhysics->applyForceToCenter({0.f, -5.f});
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space)) {
            if (onGround) {
                onGround = false;
                playerPhysics->applyImpulseToCenter({0.f, -1.5f});
            }
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::R)) {
            if (teleportCooldown >= 2.f) { playerPhysics->teleport({400.f, 300.f}); }
        }
    }

private:
    bl::gfx::Rectangle floor;
    bl::gfx::Rectangle playerBox;
    bl::gfx::Circle ball;
    bl::com::Physics2D* playerPhysics;
    bool onGround;
    float teleportCooldown;

    virtual void process(const bl::sys::Physics2D::EntityCollisionBeginEvent&) override {
        onGround = true;
    }

    virtual void process(const bl::sys::Physics2D::EntityCollisionEndEvent&) override {
        onGround = false;
    }
};

class Physics2DExample : public bl::game::Game {
private:
    bool performEarlyStartup(int, char**) override {
        // noop
        return true;
    }

    bl::engine::Settings createStartupParameters() override {
        return bl::engine::Settings().withRenderer(bl::rc::CreationSettings().withWindowSettings(
            bl::rc::WindowSettings()
                .withVideoMode(sf::VideoMode({800, 600}, 32))
                .withStyle(sf::Style::Close | sf::Style::Titlebar | sf::Style::Resize)
                .withTitle("Physics2D Demo")
                .withLetterBoxOnResize(true)));
    }

    bool completeStartup(bl::engine::Engine&) override {
        // noop
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
