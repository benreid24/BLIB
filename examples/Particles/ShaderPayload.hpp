#ifndef SHADERPAYLOAD_HPP
#define SHADERPAYLOAD_HPP

#include <BLIB/Random/Random.hpp>
#include <glm/glm.hpp>

struct ShaderPayload {
    glm::vec4 colorMultiplier;

    ShaderPayload()
    : colorMultiplier(1.f, 1.f, 1.f, 1.f) {}

    void shuffle() {
        colorMultiplier.x = bl::rand::Random::get<float>(0.f, 1.f);
        colorMultiplier.y = bl::rand::Random::get<float>(0.f, 1.f);
        colorMultiplier.z = bl::rand::Random::get<float>(0.f, 1.f);
    }
};

#endif
