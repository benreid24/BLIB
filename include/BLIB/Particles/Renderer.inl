#ifndef BLIB_PARTICLES_RENDERER_INL
#define BLIB_PARTICLES_RENDERER_INL

#include <BLIB/Particles/Renderer.hpp>

#include <BLIB/Engine/Engine.hpp>
#include <BLIB/Systems/DrawableSystem.hpp>

namespace bl
{
namespace pcl
{
template<typename T>
void Renderer<T>::init(engine::Engine& e) {
    using TEngineSystem = sys::DrawableSystem<TComponent>;

    engine = &e;
    system =
        &engine->systems().registerSystem<TEngineSystem>(bl::engine::FrameStage::RenderEarlyRefresh,
                                                         bl::engine::StateMask::All,
                                                         PipelineId,
                                                         PipelineId);
}

template<typename T>
void Renderer<T>::addToScene(rc::Scene* scene) {
    using TEngineSystem = sys::DrawableSystem<TComponent>;

    entity    = engine->ecs().createEntity();
    component = engine->ecs().emplaceComponent<TComponent>(
        entity, *engine, ContainsTransparency, PipelineId);
    link = engine->ecs().emplaceComponent<Link<T>>(entity);
    static_cast<TEngineSystem*>(system)->addToScene(entity, scene, bl::rc::UpdateSpeed::Dynamic);
}

template<typename T>
void Renderer<T>::removeFromScene() {
    engine->ecs().destroyEntity(entity);
}

template<typename T>
void Renderer<T>::notifyData(T* particles, std::size_t length) {
    component->drawParams.instanceCount = length;
    component->syncDrawParamsToScene();
    link->base = particles;
    link->len  = length;
}

} // namespace pcl
} // namespace bl

#endif
