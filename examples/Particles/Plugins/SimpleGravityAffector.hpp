#ifndef SIMPLEGRAVITYAFFECTOR_HPP
#define SIMPLEGRAVITYAFFECTOR_HPP

#include "../Constants.hpp"
#include "../Particle.hpp"
#include <BLIB/Particles/Affector.hpp>
#include <BLIB/Util/Random.hpp>

class SimpleGravityAffector : public bl::pcl::Affector<Particle> {
public:
    static constexpr float MinAccel  = 25.f;
    static constexpr float MaxAccel  = 125.f;
    static constexpr float MinRadius = 20.f;
    static constexpr float MaxRadius = 120.f;

    SimpleGravityAffector(const glm::vec2& pos)
    : pos(pos)
    , radius(bl::util::Random::get<float>(MinRadius, MaxRadius)) {}

    virtual ~SimpleGravityAffector() = default;

    virtual void update(std::span<Particle> particles, float dt, float) override {
        const float rsqrd = radius * radius;

        for (Particle& p : particles) {
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
    const glm::vec2 pos;
    const float radius;
};

#endif