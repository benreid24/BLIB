#include <BLIB/Cameras/2D/Camera2D.hpp>
#include <BLIB/Engine.hpp>
#include <BLIB/Particles.hpp>
#include <BLIB/Render.hpp>

#include "Constants.hpp"
#include "DescriptorFactory.hpp"
#include "DescriptorSet.hpp"
#include "Particle.hpp"
#include "Renderer.hpp"

#include "Plugins/SimpleTimedEmitter.hpp"

using SimpleParticleSystem = bl::pcl::ParticleManager<Particle>;

class DemoState : public bl::engine::State {
public:
    DemoState(bl::engine::Engine& engine)
    : State(bl::engine::StateMask::All) {
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
                             bl::rc::Config::ShaderIds::SkinnedMeshFragment)
                .withPrimitiveType(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST)
                .withRasterizer(rasterizer)
                .withDepthStencilState(&depthStencilDepthEnabled)
                .addDescriptorSet<bl::rc::ds::TexturePoolFactory>()
                .addDescriptorSet<bl::rc::ds::Scene3DFactory>()
                .addDescriptorSet<DescriptorFactory>()
                .build());
    }

    virtual const char* name() const override { return "DemoState"; }

    virtual void activate(bl::engine::Engine& engine) override {
        auto& simpleManager =
            engine.particleSystem().getUniqueSystem<bl::pcl::ParticleManager<Particle>>();

        simpleManager.addEmitter<SimpleTimedEmitter>();

        auto& observer = engine.renderer().getObserver(0);
        auto scene     = observer.pushScene<bl::rc::scene::Scene2D>();
        observer.setCamera<bl::cam::Camera2D>(
            sf::FloatRect(Bounds.x, Bounds.y, Bounds.z, Bounds.w));

        simpleManager.addToScene(scene);
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
 *   - Add particles. Start static to test rendering then use particle system to make interesting
 *
 * Genericize:
 *   - Make ECS drawable component generic (already close)
 *   - Make Renderer generic? Maybe just need some params
 *   - Template DescriptorSet and move into library
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
