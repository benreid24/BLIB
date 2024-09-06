#ifndef BLIB_PARTICLES_RENDERER_INL
#define BLIB_PARTICLES_RENDERER_INL

#include <BLIB/Engine/Engine.hpp>
#include <BLIB/Particles/Renderer.hpp>

namespace bl
{
namespace pcl
{
template<typename T>
Renderer<T>::Renderer()
: engine(nullptr)
, system(nullptr)
, entity(ecs::InvalidEntity)
, component(nullptr)
, link(nullptr) {}

template<typename T>
void Renderer<T>::init(engine::Engine& e) {
    using TEngineSystem = sys::DrawableSystem<TComponent>;

    if constexpr (RenderConfigMap<T>::CreateRenderPipeline) {
        static bool created = false;
        if (!created) {
            created = true;

            VkPipelineDepthStencilStateCreateInfo depthStencil{};
            depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
            depthStencil.depthTestEnable =
                RenderConfigMap<T>::EnableDepthTesting ? VK_TRUE : VK_FALSE;
            depthStencil.depthWriteEnable      = depthStencil.depthTestEnable;
            depthStencil.depthCompareOp        = VK_COMPARE_OP_LESS_OR_EQUAL;
            depthStencil.depthBoundsTestEnable = VK_FALSE;
            depthStencil.minDepthBounds        = 0.0f; // Optional
            depthStencil.maxDepthBounds        = 1.0f; // Optional
            depthStencil.stencilTestEnable     = VK_FALSE;
            depthStencil.front                 = {}; // Optional (Stencil)
            depthStencil.back                  = {}; // Optional (Stencil)

            VkPipelineRasterizationStateCreateInfo rasterizer{};
            rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
            rasterizer.depthClampEnable        = VK_FALSE;
            rasterizer.rasterizerDiscardEnable = VK_FALSE;
            rasterizer.polygonMode             = VK_POLYGON_MODE_FILL;
            rasterizer.lineWidth               = 1.0f;
            rasterizer.cullMode                = VK_CULL_MODE_NONE;
            rasterizer.frontFace               = VK_FRONT_FACE_COUNTER_CLOCKWISE;
            rasterizer.depthBiasEnable         = VK_FALSE;
            rasterizer.depthBiasConstantFactor = 0.0f; // Optional
            rasterizer.depthBiasClamp          = 0.0f; // Optional
            rasterizer.depthBiasSlopeFactor    = 0.0f; // Optional

            using DescriptorList = typename RenderConfigMap<T>::DescriptorSets;

            bl::rc::vk::PipelineParameters params(RenderConfigMap<T>::RenderPassIds);
            params.withShaders(RenderConfigMap<T>::VertexShader,
                               RenderConfigMap<T>::FragmentShader);
            params.withPrimitiveType(RenderConfigMap<T>::Topology);
            DescriptorList::addParameters(params);
            params.withRasterizer(rasterizer);
            params.withDepthStencilState(&depthStencil);
            e.renderer().pipelineCache().createPipline(PipelineId, params.build());
        }
    }

    engine    = &e;
    component = nullptr;
}

template<typename T>
void Renderer<T>::addToScene(rc::Scene* scene) {
    using TEngineSystem = sys::DrawableSystem<TComponent>;

    // TODO - WORLD_UPDATE - get world index here
    entity    = engine->ecs().createEntity(0);
    component = engine->ecs().emplaceComponent<TComponent>(
        entity, *engine, ContainsTransparency, PipelineId);
    link          = engine->ecs().emplaceComponent<Link<T>>(entity);
    link->globals = &globals;
    component->addToSceneWithPipeline(
        engine->ecs(), entity, scene, bl::rc::UpdateSpeed::Dynamic, PipelineId);
}

template<typename T>
void Renderer<T>::removeFromScene() {
    engine->ecs().destroyEntity(entity);
    component = nullptr;
}

template<typename T>
void Renderer<T>::notifyData(T* particles, std::size_t length) {
    if (component) {
        component->getDrawParametersForEdit().instanceCount = length;
        component->syncDrawParamsToScene();
        link->base = particles;
        link->len  = length;
    }
}

template<typename T>
typename Renderer<T>::TComponent* Renderer<T>::getComponent() {
    return engine->ecs().getComponent<TComponent>(entity);
}

template<typename T>
Link<T>* Renderer<T>::getLink() {
    return engine->ecs().getComponent<Link<T>>(entity);
}

template<typename T>
void Renderer<T>::draw(rc::scene::CodeScene::RenderContext& ctx) {
    ctx.renderObject(*component);
}

} // namespace pcl
} // namespace bl

#endif
