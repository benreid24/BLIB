#include <BLIB/Cameras/2D/Camera2D.hpp>
#include <BLIB/Engine.hpp>
#include <BLIB/Graphics.hpp>
#include <BLIB/Particles.hpp>
#include <BLIB/Render.hpp>

#include "Constants.hpp"
#include "Particle.hpp"
#include "ShaderPayload.hpp"

#include "Plugins/SimpleBlackHoleSink.hpp"
#include "Plugins/SimpleGravityAffector.hpp"
#include "Plugins/SimpleMovableUpdater.hpp"
#include "Plugins/SimplePointEmitter.hpp"
#include "Plugins/SimpleVelocityAffector.hpp"
#include "Plugins/SimpleWrapAffector.hpp"

// We are using the default renderer. All we need to do is specialize this class and specify our
// render settings (pipeline, transparency, shaders, etc)
namespace bl
{
namespace pcl
{
template<>
struct RenderConfigMap<Particle> {
    static constexpr std::uint32_t PipelineId  = ParticlePipelineId;
    static constexpr bool ContainsTransparency = false;

    static constexpr bool CreateRenderPipeline = true; // set to true to create pipeline for us

    // Below settings only used when pipeline is created

    static constexpr std::initializer_list<std::uint32_t> RenderPassIds =
        bl::pcl::RenderConfigDefaults<Particle>::RenderPassIds;

    using GlobalShaderPayload = ShaderPayload;
    using DescriptorSets =
        bl::pcl::RenderConfigDescriptorList<bl::rc::ds::Scene2DFactory,
                                            // This descriptor set can be used most of the time
                                            bl::pcl::DescriptorSetFactory<Particle, GpuParticle>>;

    static constexpr bool EnableDepthTesting      = true;
    static constexpr VkPrimitiveTopology Topology = VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
    static constexpr const char* VertexShader     = "Resources/Shaders/particle.vert.spv";
    static constexpr const char* FragmentShader   = "Resources/Shaders/particle.frag.spv";
};
} // namespace pcl
} // namespace bl

class InputHandler : public bl::event::Listener<sf::Event> {
public:
    InputHandler()
    : eng(nullptr)
    , particles(nullptr) {}

    void init(bl::engine::Engine& engine, bl::engine::World& world,
              bl::pcl::ParticleManager<Particle>& manager, bl::rc::Scene* s) {
        particles = &manager;
        eng       = &engine;
        worldPtr  = &world;
        scene     = s;
        globals   = &manager.getRenderer().getGlobals();
    }

    virtual void observe(const sf::Event& event) override {
        if (event.type == sf::Event::MouseButtonPressed) {
            const glm::vec2 pos(event.mouseButton.x, event.mouseButton.y);

            if (event.mouseButton.button == sf::Mouse::Button::Left) {
                particles->addEmitter<SimplePointEmitter>(pos, *worldPtr, scene);
            }
            else if (event.mouseButton.button == sf::Mouse::Button::Right) {
                particles->addAffector<SimpleGravityAffector>(pos, *worldPtr, scene);
            }
            else if (event.mouseButton.button == sf::Mouse::Button::Middle) {
                particles->addSink<SimpleBlackHoleSink>(pos, *worldPtr, scene);
            }
        }
        else if (event.type == sf::Event::MouseWheelScrolled) {
            const float factor = event.mouseWheelScroll.delta > 0.f ? 1.05f : 0.95f;
            eng->setTimeScale(eng->getTimeScale() * factor);
        }
        else if (event.type == sf::Event::KeyPressed) {
            if (event.key.code == sf::Keyboard::C) { globals->shuffle(); }
            else if (event.key.code == sf::Keyboard::R) {
                globals->colorMultiplier = glm::vec4(1.f);
            }
        }
    }

    virtual ~InputHandler() = default;

private:
    bl::engine::Engine* eng;
    bl::engine::World* worldPtr;
    bl::pcl::ParticleManager<Particle>* particles;
    bl::rc::Scene* scene;
    ShaderPayload* globals;
};

class DemoState : public bl::engine::State {
public:
    DemoState(bl::engine::Engine& engine)
    : State(bl::engine::StateMask::All) {}

    virtual const char* name() const override { return "DemoState"; }

    virtual void activate(bl::engine::Engine& engine) override {
        auto world =
            engine.getPlayer().enterWorld<bl::engine::BasicWorld<bl::rc::scene::Scene2D>>();
        auto& observer = engine.renderer().getObserver(0);
        auto scene     = world->scene();
        observer.setCamera<bl::cam::Camera2D>(
            sf::FloatRect(Bounds.x, Bounds.y, Bounds.z, Bounds.w));
        observer.setClearColor({0.05f, 0.05f, 0.05f, 1.f});

        auto& simpleManager = engine.particleSystem().getUniqueSystem<Particle>(*world);

        simpleManager.addUpdater<SimpleMovableUpdater>();
        simpleManager.addEmitter<SimplePointEmitter>(glm::vec2{400.f, 300.f}, *world, scene);
        simpleManager.addAffector<SimpleVelocityAffector>();
        simpleManager.addAffector<SimpleWrapAffector>();

        spawner.init(engine, *world, simpleManager, scene);
        bl::event::Dispatcher::subscribe(&spawner);
        simpleManager.addToScene(scene);
    }

    virtual void deactivate(bl::engine::Engine& engine) override {
        bl::event::Dispatcher::unsubscribe(&spawner);
        engine.particleSystem().removeUniqueSystem<Particle>();
        engine.getPlayer().leaveWorld();
    }

    virtual void update(bl::engine::Engine&, float, float) override {
        // nothing
    }

private:
    InputHandler spawner;
};

int main() {
    bl::cam::OverlayCamera::setOverlayCoordinateSpace(800.f, 600.f);

    const bl::engine::Settings engineSettings = bl::engine::Settings().withWindowParameters(
        bl::engine::Settings::WindowParameters()
            .withVideoMode(sf::VideoMode(800, 600, 32))
            .withStyle(sf::Style::Close | sf::Style::Titlebar | sf::Style::Resize)
            .withTitle("Particle System Demo")
            .withLetterBoxOnResize(true));
    bl::engine::Engine engine(engineSettings);

    engine.run(std::make_shared<DemoState>(engine));

    return 0;
}
