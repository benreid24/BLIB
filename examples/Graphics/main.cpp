#include <BLIB/Cameras.hpp>
#include <BLIB/Components.hpp>
#include <BLIB/Engine.hpp>
#include <BLIB/Graphics.hpp>
#include <BLIB/Render.hpp>
#include <BLIB/Systems.hpp>
#include <iostream>

class DemoState : public bl::engine::State {
public:
    DemoState()
    : State(bl::engine::StateMask::All)
    , scene(nullptr) {}

    virtual ~DemoState() = default;

private:
    bl::rc::Scene* scene;
    bl::gfx::Slideshow slideshow;
    bl::gfx::Animation2D animation;
    bl::gfx::Rectangle rectangle;
    bl::gfx::Circle circle;
    bl::gfx::Triangle triangle;

    virtual const char* name() const override { return "DemoState"; }

    virtual void activate(bl::engine::Engine& engine) override {
        // create scene and camera
        auto& o = engine.renderer().getObserver();
        scene   = o.pushScene<bl::rc::scene::BatchedScene>();
        o.setCamera<bl::cam::Camera2D>(sf::FloatRect{0.f, 0.f, 1920.f, 1080.f});
        o.setClearColor({1.f, 1.f, 1.f, 1.f});

        // add slideshow animation to scene
        slideshow.createWithUniquePlayer(
            engine,
            bl::resource::ResourceManager<bl::gfx::a2d::AnimationData>::load(
                "resources/water.anim"),
            true,
            true);
        slideshow.getTransform().setPosition({300.f, 300.f});
        slideshow.addToScene(scene, bl::rc::UpdateSpeed::Static);

        // add standard animation to scene
        animation.createWithUniquePlayer(
            engine,
            bl::resource::ResourceManager<bl::gfx::a2d::AnimationData>::load(
                "resources/animation.anim"),
            true,
            true);
        animation.getTransform().setPosition({700.f, 500.f});
        animation.addToScene(scene, bl::rc::UpdateSpeed::Static);

        // add rectangle to scene
        rectangle.create(engine, {100.f, 120.f});
        rectangle.setFillColor({1.f, 0.f, 0.f, 1.f});
        rectangle.setOutlineColor({0.f, 0.f, 0.f, 1.f});
        rectangle.setOutlineThickness(2.f);
        rectangle.getTransform().setPosition({1000.f, 700.f});
        rectangle.addToScene(scene, bl::rc::UpdateSpeed::Static);

        // add circle to scene
        circle.create(engine, 75.f);
        circle.setFillColor({0.f, 1.f, 0.f, 1.f});
        circle.setOutlineColor({0.f, 0.f, 0.f, 1.f});
        circle.setOutlineThickness(3.f);
        circle.getTransform().setPosition({1300.f, 120.f});
        circle.addToScene(scene, bl::rc::UpdateSpeed::Static);

        // add triangle to scene
        triangle.create(engine, {50.f, 0.f}, {100.f, 70.f}, {0.f, 70.f});
        triangle.setFillColor({0.f, 0.f, 1.f, 1.f});
        triangle.setOutlineColor({0.f, 0.f, 0.f, 1.f});
        triangle.setOutlineThickness(3.f);
        triangle.getTransform().setPosition({1500.f, 250.f});
        triangle.addToScene(scene, bl::rc::UpdateSpeed::Static);
    }

    virtual void deactivate(bl::engine::Engine& engine) override {
        engine.renderer().getObserver().popScene();
    }

    virtual void update(bl::engine::Engine&, float) override {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::G)) {
            rectangle.setHorizontalColorGradient({0.f, 0.f, 1.f, 1.f}, {0.f, 1.f, 0.f, 1.f});
            circle.setColorGradient({1.f, 0.f, 0.f, 1.f}, {0.f, 0.f, 1.f, 1.f});
            triangle.setColorGradient(
                {1.f, 0.f, 0.f, 1.f}, {0.f, 1.f, 0.f, 1.f}, {0.f, 0.f, 1.f, 1.f});
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::B)) {
            rectangle.removeColorGradient();
            circle.removeColorGradient();
            triangle.removeColorGradient();
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::F)) {
            circle.flash(0.4f, 0.8f);
            rectangle.flash(1.f, 1.f);
            slideshow.flash(0.5f, 0.3f);
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::V)) {
            circle.stopFlashing();
            rectangle.stopFlashing();
            slideshow.stopFlashing();
        }
    }

    virtual void render(bl::engine::Engine&, float) override {
        // deprecated
    }
};

int main() {
    bl::logging::Config::configureOutput(std::cout, bl::logging::Config::Debug);
    const bl::engine::Settings engineSettings = bl::engine::Settings().withWindowParameters(
        bl::engine::Settings::WindowParameters()
            .withVideoMode(sf::VideoMode(1920, 1080, 32))
            .withStyle(sf::Style::Close | sf::Style::Titlebar | sf::Style::Resize)
            .withTitle("Renderer Demo")
            .withLetterBoxOnResize(true));
    bl::engine::Engine engine(engineSettings);

    engine.run(std::make_shared<DemoState>());

    return 0;
}
