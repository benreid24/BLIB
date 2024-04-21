#ifndef SIMPLEMOVABLEUPDATER_HPP
#define SIMPLEMOVABLEUPDATER_HPP

#include "../Particle.hpp"
#include "Movable.hpp"
#include <BLIB/Particles/MetaUpdater.hpp>

class SimpleMovableUpdater : public bl::pcl::MetaUpdater<Particle> {
public:
    virtual ~SimpleMovableUpdater() = default;

    virtual void update(Proxy& proxy, float dt, float) {
        for (auto& affector : proxy.getAffectors()) {
            Movable* m = dynamic_cast<Movable*>(affector.get());
            if (m) { m->applyVelocity(dt); }
        }

        for (auto& sink : proxy.getSinks()) {
            Movable* m = dynamic_cast<Movable*>(sink.get());
            if (m) { m->applyVelocity(dt); }
        }

        for (auto& emitter : proxy.getEmitters()) {
            Movable* m = dynamic_cast<Movable*>(emitter.get());
            if (m) { m->applyVelocity(dt); }
        }
    }
};

#endif
