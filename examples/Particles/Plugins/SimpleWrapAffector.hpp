#ifndef SIMPLEWRAPAFFECTOR_HPP
#define SIMPLEWRAPAFFECTOR_HPP

#include "../Constants.hpp"
#include "../Particle.hpp"
#include <BLIB/Particles/Affector.hpp>

class SimpleWrapAffector : public bl::pcl::Affector<Particle> {
public:
    virtual ~SimpleWrapAffector() = default;

    virtual void update(Proxy& proxy, float, float) override {
        for (Particle& p : proxy.particles()) {
            if (p.pos.x < Bounds.x) { p.pos.x += Bounds.z; }
            else if (p.pos.x > Bounds.x + Bounds.z) { p.pos.x -= Bounds.z; }
            if (p.pos.y < Bounds.y) { p.pos.y += Bounds.w; }
            else if (p.pos.y > Bounds.y + Bounds.w) { p.pos.y -= Bounds.w; }
        }
    }
};

#endif