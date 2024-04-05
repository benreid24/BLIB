#ifndef RENDERER_HPP
#define RENDERER_HPP

#include "Component.hpp"
#include "Constants.hpp"
#include "Particle.hpp"
#include <BLIB/ECS.hpp>
#include <BLIB/Engine/Engine.hpp>
#include <BLIB/Particles/Link.hpp>
#include <BLIB/Particles/Renderer.hpp>

namespace bl
{
namespace pcl
{
template<>
class Renderer<Particle> {
public:
    void init(engine::Engine& e) { engine = &e; }

    void addToScene(rc::Scene* scene) {
        constexpr std::uint32_t pipeline = ParticlePipelineId; // TODO - actual pipeline
        constexpr bool transparent       = false;              // TODO - parameterize
        constexpr glm::vec2 size         = {2.f, 2.f};         // TODO - parameterize

        entity    = engine->ecs().createEntity();
        component = engine->ecs().emplaceComponent<Component>(
            entity, engine->renderer(), size, transparent, pipeline);
        link = engine->ecs().emplaceComponent<Link<Particle>>(entity);
    }

    void removeFromScene() { engine->ecs().destroyEntity(entity); }

    void notifyData(Particle* particles, std::size_t length) {
        component->drawParams.instanceCount = length;
        link->base                          = particles;
        link->len                           = length;
    }

private:
    engine::Engine* engine;
    ecs::Entity entity;
    Component* component;
    Link<Particle>* link;
};

} // namespace pcl
} // namespace bl

#endif
