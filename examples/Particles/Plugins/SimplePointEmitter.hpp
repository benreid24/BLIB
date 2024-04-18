#ifndef SIMPLEPOINTEMITTER_HPP
#define SIMPLEPOINTEMITTER_HPP

#include "../Constants.hpp"
#include "../Particle.hpp"
#include <BLIB/Math.hpp>
#include <BLIB/Particles/Emitter.hpp>
#include <BLIB/Util/Random.hpp>

class SimplePointEmitter : public bl::pcl::Emitter<Particle> {
public:
    static constexpr float MinRotRate        = bl::math::degreesToRadians(20.f);
    static constexpr float MaxRotRate        = bl::math::degreesToRadians(200.f);
    static constexpr float MinVel            = 20.f;
    static constexpr float MaxVel            = 150.f;
    static constexpr float MinSpawnRate      = 2.f;
    static constexpr float MaxSpawnRate      = 30.f;
    static constexpr unsigned int SpawnCount = 100;

    SimplePointEmitter(const glm::vec2& pos)
    : pos(pos)
    , angle(bl::util::Random::get<float>(0.f, 2.f * bl::math::Pi))
    , angularVel(makeAngularVel())
    , spawnRate(bl::util::Random::get<float>(MinSpawnRate, MaxSpawnRate))
    , spawnResidual(0.f)
    , spawned(0) {}

    virtual ~SimplePointEmitter() = default;

    virtual void update(const Emitter::Proxy& proxy, float dt, float) override {
        angle += angularVel * dt;
        if (angularVel > 0.f) {
            while (angle >= bl::math::Tau) { angle -= bl::math::Tau; }
        }
        else {
            while (angle <= bl::math::Tau) { angle += bl::math::Tau; }
        }

        spawnResidual += spawnRate * dt;
        const float pcos = std::cos(angle);
        const float psin = std::sin(angle);
        while (spawned < SpawnCount && spawnResidual >= 1.f) {
            const float vel = bl::util::Random::get<float>(MinVel, MaxVel);
            proxy.emit(pos, glm::vec2(vel * pcos, vel * psin));
            spawnResidual -= 1.f;
            ++spawned;
        }

        // TODO - count spawned and erase self
    }

private:
    const glm::vec2 pos;
    float angle;
    const float angularVel;
    const float spawnRate;
    float spawnResidual;
    unsigned int spawned;

    static float makeAngularVel() {
        const float rate = bl::util::Random::get<float>(MinRotRate, MaxRotRate);
        if (bl::util::Random::chance(50, 100)) { return -rate; }
        return rate;
    }
};

#endif
