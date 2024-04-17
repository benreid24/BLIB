#ifndef SIMPLETIMEDEMITTER_HPP
#define SIMPLETIMEDEMITTER_HPP

#include "../Constants.hpp"
#include "../Particle.hpp"
#include <BLIB/Particles/Emitter.hpp>
#include <BLIB/Util/Random.hpp>

class SimpleTimedEmitter : public bl::pcl::Emitter<Particle> {
public:
    static constexpr float SpawnRate = 100.f;
    static constexpr int MaxSpawn    = 5000;

    SimpleTimedEmitter()
    : spawned(0)
    , residual(0.f) {}

    virtual ~SimpleTimedEmitter() = default;

    virtual void update(const Emitter::Proxy& proxy, float dt, float realDt) override {
        if (spawned < MaxSpawn) {
            residual += SpawnRate * dt;
            while (residual >= 1.f && spawned < MaxSpawn) {
                proxy.emit(glm::vec2(bl::util::Random::get<float>(Bounds.x, Bounds.z),
                                     bl::util::Random::get<float>(Bounds.y, Bounds.w)));

                residual -= 1.f;
                ++spawned;
            }
        }
    }

private:
    int spawned;
    float residual;
};

#endif
