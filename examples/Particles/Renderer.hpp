#ifndef RENDERER_HPP
#define RENDERER_HPP

#include "Component.hpp"
#include "Constants.hpp"
#include "Particle.hpp"
#include <BLIB/ECS.hpp>
#include <BLIB/Engine/Engine.hpp>
#include <BLIB/Particles/Link.hpp>
#include <BLIB/Particles/Renderer.hpp>
#include <BLIB/Systems/DrawableSystem.hpp>

namespace bl
{
namespace pcl
{
template<>
class Renderer<Particle> {
public:
    using Sys = bl::sys::DrawableSystem<ExampleComponent>;

    void init(engine::Engine& e) {
        engine = &e;
        system = &engine->systems().registerSystem<Sys>(bl::engine::FrameStage::RenderEarlyRefresh,
                                                        bl::engine::StateMask::All,
                                                        ParticlePipelineId,
                                                        ParticlePipelineId);
    }

    void addToScene(rc::Scene* scene) {
        constexpr std::uint32_t pipeline = ParticlePipelineId; // TODO - actual pipeline
        constexpr bool transparent       = false;              // TODO - parameterize
        constexpr glm::vec2 size         = {2.f, 2.f};         // TODO - parameterize

        // TODO - texture particle (or solid color?)

        entity    = engine->ecs().createEntity();
        component = engine->ecs().emplaceComponent<ExampleComponent>(
            entity, engine->renderer(), size, transparent, pipeline);
        link = engine->ecs().emplaceComponent<Link<Particle>>(entity);
        system->addToScene(entity, scene, bl::rc::UpdateSpeed::Dynamic);
    }

    void removeFromScene() { engine->ecs().destroyEntity(entity); }

    void notifyData(Particle* particles, std::size_t length) {
        component->drawParams.instanceCount = length;
        component->syncDrawParamsToScene();
        link->base = particles;
        link->len  = length;
    }

private:
    engine::Engine* engine;
    Sys* system;
    ecs::Entity entity;
    ExampleComponent* component;
    Link<Particle>* link;
};

} // namespace pcl
} // namespace bl

#endif
