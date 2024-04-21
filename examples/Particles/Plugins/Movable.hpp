#ifndef MOVABLE_HPP
#define MOVABLE_HPP

#include "../Constants.hpp"
#include <BLIB/Util/Random.hpp>
#include <glm/glm.hpp>

class Movable {
public:
    virtual ~Movable() = default;

    void applyVelocity(float dt) {
        pos += vel * dt;
        if (pos.x < Bounds.x) { pos.x += Bounds.z; }
        else if (pos.x > Bounds.x + Bounds.z) { pos.x -= Bounds.z; }
        if (pos.y < Bounds.y) { pos.y += Bounds.w; }
        else if (pos.y > Bounds.y + Bounds.w) { pos.y -= Bounds.w; }
    }

protected:
    glm::vec2 pos;

    Movable(const glm::vec2& pos)
    : pos(pos)
    , vel(bl::util::Random::get<float>(-50.f, 50.f), bl::util::Random::get<float>(-50.f, 50.f)) {}

private:
    const glm::vec2 vel;
};

#endif
