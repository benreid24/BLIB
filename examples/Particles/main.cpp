#include <BLIB/Cameras/2D/Camera2D.hpp>
#include <BLIB/Engine.hpp>
#include <BLIB/Graphics.hpp>
#include <BLIB/Particles.hpp>
#include <BLIB/Render.hpp>

#include "Constants.hpp"
#include "DescriptorFactory.hpp"
#include "DescriptorSet.hpp"
#include "Particle.hpp"
#include "Renderer.hpp"

#include "Plugins/SimpleBlackHoleSink.hpp"
#include "Plugins/SimpleGravityAffector.hpp"
#include "Plugins/SimplePointEmitter.hpp"
#include "Plugins/SimpleVelocityAffector.hpp"
#include "Plugins/SimpleWrapAffector.hpp"

using SimpleParticleSystem = bl::pcl::ParticleManager<Particle>;

bool pipelineCreated = false;

class ClickSpawner : public bl::event::Listener<sf::Event> {
public:
    ClickSpawner()
    : eng(nullptr)
    , particles(nullptr) {}

    void init(bl::engine::Engine& engine, bl::pcl::ParticleManager<Particle>& manager,
              bl::rc::Scene* s) {
        particles = &manager;
        eng       = &engine;
        scene     = s;
    }

    virtual void observe(const sf::Event& event) override {
        if (event.type == sf::Event::MouseButtonPressed) {
            const glm::vec2 pos(event.mouseButton.x, event.mouseButton.y);

            if (event.mouseButton.button == sf::Mouse::Button::Left) {
                particles->addEmitter<SimplePointEmitter>(pos, *eng, scene);
            }
            else if (event.mouseButton.button == sf::Mouse::Button::Right) {
                particles->addAffector<SimpleGravityAffector>(pos, *eng, scene);
            }
            else if (event.mouseButton.button == sf::Mouse::Button::Middle) {
                particles->addSink<SimpleBlackHoleSink>(pos, *eng, scene);
            }
        }
    }

    virtual ~ClickSpawner() = default;

private:
    bl::engine::Engine* eng;
    bl::pcl::ParticleManager<Particle>* particles;
    bl::rc::Scene* scene;
};

class DemoState : public bl::engine::State {
public:
    DemoState(bl::engine::Engine& engine)
    : State(bl::engine::StateMask::All) {}

    virtual const char* name() const override { return "DemoState"; }

    virtual void activate(bl::engine::Engine& engine) override {
        if (!pipelineCreated) {
            pipelineCreated = true;
            createPipeline(engine);
        }

        auto& observer = engine.renderer().getObserver(0);
        auto scene     = observer.pushScene<bl::rc::scene::Scene2D>();
        observer.setCamera<bl::cam::Camera2D>(
            sf::FloatRect(Bounds.x, Bounds.y, Bounds.z, Bounds.w));
        observer.setClearColor({0.05f, 0.05f, 0.05f, 1.f});

        // TODO - better interface
        auto& simpleManager =
            engine.particleSystem().getUniqueSystem<bl::pcl::ParticleManager<Particle>>();

        simpleManager.addEmitter<SimplePointEmitter>(glm::vec2{400.f, 300.f}, engine, scene);
        simpleManager.addAffector<SimpleVelocityAffector>();
        simpleManager.addAffector<SimpleWrapAffector>();

        spawner.init(engine, simpleManager, scene);
        bl::event::Dispatcher::subscribe(&spawner);
        simpleManager.addToScene(scene);
    }

    virtual void deactivate(bl::engine::Engine& engine) override {
        bl::event::Dispatcher::unsubscribe(&spawner);
        engine.particleSystem().removeUniqueSystem<bl::pcl::ParticleManager<Particle>>();
        engine.renderer().getObserver().popScene();
    }

    virtual void update(bl::engine::Engine&, float, float) override {
        // nothing
    }

private:
    ClickSpawner spawner;

    void createPipeline(bl::engine::Engine& engine) {
        // create custom pipeline to render Particle
        VkPipelineDepthStencilStateCreateInfo depthStencilDepthEnabled{};
        depthStencilDepthEnabled.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
        depthStencilDepthEnabled.depthTestEnable       = VK_TRUE;
        depthStencilDepthEnabled.depthWriteEnable      = VK_TRUE;
        depthStencilDepthEnabled.depthCompareOp        = VK_COMPARE_OP_LESS_OR_EQUAL;
        depthStencilDepthEnabled.depthBoundsTestEnable = VK_FALSE;
        depthStencilDepthEnabled.minDepthBounds        = 0.0f; // Optional
        depthStencilDepthEnabled.maxDepthBounds        = 1.0f; // Optional
        depthStencilDepthEnabled.stencilTestEnable     = VK_FALSE;
        depthStencilDepthEnabled.front                 = {}; // Optional (Stencil)
        depthStencilDepthEnabled.back                  = {}; // Optional (Stencil)

        VkPipelineRasterizationStateCreateInfo rasterizer{};
        rasterizer.sType            = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterizer.depthClampEnable = VK_FALSE;
        rasterizer.rasterizerDiscardEnable = VK_FALSE;
        rasterizer.polygonMode             = VK_POLYGON_MODE_FILL;
        rasterizer.lineWidth               = 1.0f;
        rasterizer.cullMode                = VK_CULL_MODE_NONE;
        rasterizer.frontFace               = VK_FRONT_FACE_COUNTER_CLOCKWISE;
        rasterizer.depthBiasEnable         = VK_FALSE;
        rasterizer.depthBiasConstantFactor = 0.0f; // Optional
        rasterizer.depthBiasClamp          = 0.0f; // Optional
        rasterizer.depthBiasSlopeFactor    = 0.0f; // Optional

        engine.renderer().pipelineCache().createPipline(
            ParticlePipelineId,
            bl::rc::vk::PipelineParameters(
                {bl::rc::Config::RenderPassIds::StandardAttachmentDefault,
                 bl::rc::Config::RenderPassIds::SwapchainDefault})
                .withShaders("Resources/Shaders/particle.vert.spv",
                             "Resources/Shaders/particle.frag.spv")
                .withPrimitiveType(VK_PRIMITIVE_TOPOLOGY_POINT_LIST)
                .withRasterizer(rasterizer)
                .withDepthStencilState(&depthStencilDepthEnabled)
                .addDescriptorSet<bl::rc::ds::Scene2DFactory>()
                .addDescriptorSet<DescriptorFactory>()
                .build());
    }
};

/**
 * TODO
 * Functionality:
 *   - Special particles for emitters and sinks
 *   - Allow emitters, sinks, affectors to erase themselves
 *   - Add meta updaters to particle systems
 *   - Make particle system interface less verbose
 *
 * Genericize:
 *   - Make ECS drawable component generic (already close)
 *   - Make Renderer generic? Maybe just need some params
 *   - Template DescriptorSet and move into library
 *   - Add global info binding struct to descriptor set
 *   - Add way to add additional data (textures, etc) to descriptor set w/o rewrite
 *   - Simplify interface of ParticleSystem to allow fetch/create using just particle type
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

    engine.run(std::make_shared<DemoState>(engine));

    return 0;
}
