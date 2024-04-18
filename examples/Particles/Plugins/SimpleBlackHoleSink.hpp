#ifndef SIMPLEBLACKHOLESINK_HPP
#define SIMPLEBLACKHOLESINK_HPP

#include "../Particle.hpp"
#include <BLIB/Particles/Sink.hpp>
#include <BLIB/Util/Random.hpp>

class SimpleBlackHoleSink : public bl::pcl::Sink<Particle> {
public:
    static constexpr float MinRadius = 20.f;
    static constexpr float MaxRadius = 50.f;
    static constexpr float MinAccel  = 75.f;
    static constexpr float EatRadius = 4.f;

    SimpleBlackHoleSink(const glm::vec2& pos)
    : pos(pos)
    , radius(bl::util::Random::get<float>(MinRadius, MaxRadius)) {}

    virtual ~SimpleBlackHoleSink() = default;

    virtual void update(const Proxy& proxy, std::span<Particle> particles, float dt,
                        float) override {
        const float rsqrd = radius * radius;

        for (Particle& p : particles) {
            const glm::vec2 diff = pos - p.pos;
            const float distSqrd = diff.x * diff.x + diff.y * diff.y;

            if (distSqrd > rsqrd) { continue; }

            if (distSqrd <= EatRadius * EatRadius) { proxy.destroy(p); }
            else {
                const float accel   = MinAccel * rsqrd / distSqrd;
                const glm::vec2 dir = glm::normalize(diff);
                p.vel += dir * accel * dt;
            }
        }
    }

private:
    const glm::vec2 pos;
    const float radius;
};

#endif
