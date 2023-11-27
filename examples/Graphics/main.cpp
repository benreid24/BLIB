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

    bl::gfx::BatchedShapes2D batched;
    bl::gfx::BatchRectangle batchRect;
    bl::gfx::BatchRectangle batchRect2;
    bl::gfx::BatchCircle batchCircle;
    bl::gfx::BatchTriangle batchTriangle;

    virtual const char* name() const override { return "DemoState"; }

    virtual void activate(bl::engine::Engine& engine) override {
        // create scene and camera
        auto& o = engine.renderer().getObserver();
        scene   = o.pushScene<bl::rc::scene::Scene2D>();
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

        // add a set of batched shapes to the scene
        batched.create(engine, 128);
        batched.getTransform().setPosition({100.f, 800.f});
        batched.addToScene(scene, bl::rc::UpdateSpeed::Static);

        batchRect.create(engine, batched, {120.f, 25.f});
        batchRect.setFillColor({0.f, 1.f, 0.f, 1.f});
        batchRect.setOutlineColor({0.f, 0.f, 0.f, 1.f});
        batchRect.setOutlineThickness(3.f);

        batchRect2.create(engine, batched, {120.f, 25.f});
        batchRect2.setFillColor({0.f, 0.f, 1.f, 1.f});
        batchRect2.setOutlineColor({0.f, 0.f, 0.f, 1.f});
        batchRect2.setOutlineThickness(3.f);
        batchRect2.getLocalTransform().setPosition({0.f, batchRect.getLocalBounds().height});

        batchCircle.create(engine, batched, 45.f);
        batchCircle.setFillColor({0.2f, 0.2f, 0.2f, 1.f});
        batchCircle.setOutlineColor({1.f, 0.f, 0.f, 1.f});
        batchCircle.setOutlineThickness(2.f);
        batchCircle.getLocalTransform().setPosition({batchRect.getLocalBounds().width + 50.f, 0.f});

        batchTriangle.create(engine, batched, {0.f, 0.f}, {40.f, 0.f}, {20.f, 40.f});
        batchTriangle.setFillColor({0.7f, 0.6f, 0.1f, 1.f});
        batchTriangle.setOutlineColor({0.f, 0.f, 0.f, 1.f});
        batchTriangle.setOutlineThickness(2.f);
        batchTriangle.getLocalTransform().setPosition({60.f, 95.f});
    }

    virtual void deactivate(bl::engine::Engine& engine) override {
        engine.renderer().getObserver().popScene();
    }

    virtual void update(bl::engine::Engine&, float, float) override {
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
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::R)) { rectangle.removeFromScene(); }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
            rectangle.addToScene(scene, bl::rc::UpdateSpeed::Static);
        }
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
