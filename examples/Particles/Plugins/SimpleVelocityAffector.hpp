#ifndef SIMPLEVELOCITYAFFECTOR_HPP
#define SIMPLEVELOCITYAFFECTOR_HPP

#include "../Particle.hpp"
#include <BLIB/Particles/Affector.hpp>

class SimpleVelocityAffector : public bl::pcl::Affector<Particle> {
public:
    virtual ~SimpleVelocityAffector() = default;

    virtual void update(std::span<Particle> particles, float dt, float) override {
        for (Particle& p : particles) { p.pos += p.vel * dt; }
    }
};

#endif
