#include <BLIB/Engine.hpp>

class DemoState : public bl::engine::State {
public:
    virtual ~DemoState() = default;

    virtual const char* name() const override { return "DemoState"; }

    virtual void activate(bl::engine::Engine&) override {
        // TODO - add renderable
    }

    virtual void deactivate(bl::engine::Engine&) override {}

    virtual void update(bl::engine::Engine&, float) override {}

    virtual void render(bl::engine::Engine& engine, float) override {
        engine.renderer().renderFrame();
    }
};

int main() {
    const bl::engine::Settings engineSettings = bl::engine::Settings().withWindowParameters(
        bl::engine::Settings::WindowParameters()
            .withVideoMode(sf::VideoMode(1920, 1080, 32))
            .withStyle(sf::Style::Close | sf::Style::Titlebar)
            .withTitle("Renderer Demo")
            .withIcon("vulkan.png"));
    bl::engine::Engine engine(engineSettings);

    engine.run(std::make_shared<DemoState>());

    return 0;
}
