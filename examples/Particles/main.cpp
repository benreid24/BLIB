#include <BLIB/Engine.hpp>
#include <BLIB/Particles.hpp>

#include "DescriptorFactory.hpp"
#include "DescriptorSet.hpp"
#include "Particle.hpp"
#include "Renderer.hpp"

class DemoState : public bl::engine::State {
public:
    DemoState()
    : State(bl::engine::StateMask::All) {}

    virtual const char* name() const override { return "DemoState"; }

    virtual void activate(bl::engine::Engine& engine) override {
        // TODO
    }

    virtual void deactivate(bl::engine::Engine& engine) override {
        // TODO - remove particle system
        engine.renderer().getObserver().popScene();
    }

    virtual void update(bl::engine::Engine&, float, float) override {
        // nothing
    }

private:
    // data?
};

/**
 * TODO
 * Functionality:
 *   - Make pipeline + shaders
 *   - Build main()
 *   - Add particles. Start static to test rendering then use particle system to make interesting
 *
 * Genericize:
 *   - Make ECS drawable component generic (already close)
 *   - Make Renderer generic? Maybe just need some params
 *   - Template DescriptorSet and move into library
 *   - Add way to add additional data (textures, etc) to descriptor set w/o rewrite
 */

int main() {
    bl::cam::OverlayCamera::setOverlayCoordinateSpace(800.f, 600.f);

    const bl::engine::Settings engineSettings = bl::engine::Settings().withWindowParameters(
        bl::engine::Settings::WindowParameters()
            .withVideoMode(sf::VideoMode(800, 600, 32))
            .withStyle(sf::Style::Close | sf::Style::Titlebar | sf::Style::Resize)
            .withTitle("Particle System Demo")
            .withLetterBoxOnResize(true));
    bl::engine::Engine engine(engineSettings);

    engine.run(std::make_shared<DemoState>());

    return 0;
}
