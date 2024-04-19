#ifndef PARTICLE_HPP
#define PARTICLE_HPP

#include <BLIB/Util/Random.hpp>
#include <glm/glm.hpp>

struct Particle {
    glm::vec2 pos;
    glm::vec2 vel;
    glm::vec4 color;
    float radius;
    int variant;

    enum Variant : std::int32_t { Regular, BlackHole, Emitter };

    Particle() = default;

    Particle(const glm::vec2& pos, const glm::vec2& vel, Variant variant)
    : pos(pos)
    , vel(vel)
    , color(makeColor())
    , radius(bl::util::Random::get<float>(1.f, 3.f))
    , variant(variant) {}

    static glm::vec4 makeColor() {
        return glm::vec4(bl::util::Random::get<float>(0.1f, 1.f),
                         bl::util::Random::get<float>(0.1f, 1.f),
                         bl::util::Random::get<float>(0.1f, 1.f),
                         1.f);
    }
};

struct GpuParticle {
    glm::vec2 pos;
    float radius;
    std::int32_t variant;
    glm::vec4 color;

    GpuParticle& operator=(const Particle& p) {
        pos    = p.pos;
        radius = p.radius;
        color  = p.color;
        return *this;
    }
};

#endif
