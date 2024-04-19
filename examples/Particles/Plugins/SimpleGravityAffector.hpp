#ifndef SIMPLEGRAVITYAFFECTOR_HPP
#define SIMPLEGRAVITYAFFECTOR_HPP

#include "../Constants.hpp"
#include "../Particle.hpp"
#include <BLIB/Graphics/Circle.hpp>
#include <BLIB/Particles/Affector.hpp>
#include <BLIB/Util/Random.hpp>

class SimpleGravityAffector : public bl::pcl::Affector<Particle> {
public:
    static constexpr float MinAccel  = 25.f;
    static constexpr float MaxAccel  = 125.f;
    static constexpr float MinRadius = 20.f;
    static constexpr float MaxRadius = 120.f;

    SimpleGravityAffector(const glm::vec2& pos, bl::engine::Engine& engine, bl::rc::Scene* scene)
    : pos(pos)
    , radius(bl::util::Random::get<float>(MinRadius, MaxRadius)) {
        circle.create(engine, 5.f);
        circle.setFillColor(sf::Color(79, 50, 2));
        circle.setOutlineThickness(1.f);
        circle.setOutlineColor(sf::Color(44, 140, 0));
        circle.getTransform().setPosition(pos);
        circle.getTransform().setOrigin({6.f, 6.f});
        circle.addToScene(scene, bl::rc::UpdateSpeed::Static);
    }

    virtual ~SimpleGravityAffector() = default;

    virtual void update(Proxy& proxy, float dt, float) override {
        const float rsqrd = radius * radius;

        for (Particle& p : proxy.particles()) {
            const glm::vec2 diff = pos - p.pos;
            const float distSqrd = diff.x * diff.x + diff.y * diff.y;

            if (distSqrd > rsqrd) { continue; }

            const bool isApproaching  = glm::dot(diff, p.vel) > 0.f;
            const float captureFactor = isApproaching ? 0.8f : 1.f;
            const float factor        = distSqrd / rsqrd * captureFactor;
            const float accel         = factor * (MaxAccel - MinAccel) + MinAccel;
            const glm::vec2 dir       = glm::normalize(diff);
            p.vel += dir * accel * dt;
        }
    }

private:
    bl::gfx::Circle circle;
    const glm::vec2 pos;
    const float radius;
};

#endif