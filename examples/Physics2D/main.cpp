#include <BLIB/Engine.hpp>

class DemoState : public bl::engine::State {
public:
    DemoState(bl::engine::Engine& engine)
    : State(bl::engine::StateMask::All) {}

    virtual const char* name() const override { return "DemoState"; }

    virtual void activate(bl::engine::Engine& engine) override {
        // TODO
    }

    virtual void deactivate(bl::engine::Engine& engine) override {
        // TODO
    }

    virtual void update(bl::engine::Engine&, float, float) override {
        // nothing
    }

private:
};

int main() {
    bl::cam::OverlayCamera::setOverlayCoordinateSpace(800.f, 600.f);

    const bl::engine::Settings engineSettings = bl::engine::Settings().withWindowParameters(
        bl::engine::Settings::WindowParameters()
            .withVideoMode(sf::VideoMode(800, 600, 32))
            .withStyle(sf::Style::Close | sf::Style::Titlebar | sf::Style::Resize)
            .withTitle("Physics2D Demo")
            .withLetterBoxOnResize(true));
    bl::engine::Engine engine(engineSettings);

    engine.run(std::make_shared<DemoState>(engine));
}
