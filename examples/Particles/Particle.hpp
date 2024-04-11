#ifndef PARTICLE_HPP
#define PARTICLE_HPP

#include <glm/glm.hpp>

struct Particle {
    glm::vec2 pos;
    glm::vec2 vel;

    Particle() = default;

    Particle(const glm::vec2& pos)
    : pos(pos)
    , vel(0.f, 0.f) {}

    Particle(const glm::vec2& pos, const glm::vec2& vel)
    : pos(pos)
    , vel(vel) {}
};

struct GpuParticle {
    glm::vec2 pos;

    GpuParticle& operator=(const Particle& p) { pos = p.pos; }
};

#endif
