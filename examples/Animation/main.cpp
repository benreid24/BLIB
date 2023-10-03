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

    virtual const char* name() const override { return "DemoState"; }

    virtual void activate(bl::engine::Engine& engine) override {
        // create scene and camera
        auto& o   = engine.renderer().getObserver();
        scene     = o.pushScene<bl::rc::scene::BatchedScene>();
        auto* cam = o.setCamera<bl::cam::Camera2D>(sf::FloatRect{0.f, 0.f, 1920.f, 1080.f});
        cam->setNearAndFarPlanes(-100000.f, 100000.f);
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
    }

    virtual void deactivate(bl::engine::Engine& engine) override {
        engine.renderer().getObserver().popScene();
    }

    virtual void update(bl::engine::Engine&, float) override {
        // all handled in systems
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
