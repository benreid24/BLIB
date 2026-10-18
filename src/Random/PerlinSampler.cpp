#include <BLIB/Random/PerlinSampler.hpp>

#include <BLIB/Random/Seed.hpp>

namespace bl
{
namespace rand
{
PerlinSampler::PerlinSampler(const PerlinParameters& params, const glm::vec1& origin,
                             const bl::rand::Seed& seed, std::uint64_t id)
: perlin(seed.getPerlin(id))
, origin(origin, 0.f, 0.f)
, params(params) {}

PerlinSampler::PerlinSampler(const PerlinParameters& params, const glm::vec2& origin,
                             const bl::rand::Seed& seed, std::uint64_t id)
: perlin(seed.getPerlin(id))
, origin(origin, 0.f)
, params(params) {}

PerlinSampler::PerlinSampler(const PerlinParameters& params, const glm::vec3& origin,
                             const bl::rand::Seed& seed, std::uint64_t id)
: perlin(seed.getPerlin(id))
, origin(origin)
, params(params) {}

float PerlinSampler::sample(float pos) const {
    const float xf = pos - origin.x;
    float normal =
        perlin.octave1DNormalized(xf * params.frequency, params.octaves, params.persistence);
    normal = (normal + 1.f) * 0.5f; // map to [0,1]
    return normal;
}

float PerlinSampler::sample(const glm::vec2& pos) const {
    const float xf = pos.x - origin.x;
    const float yf = pos.y - origin.y;
    float normal   = perlin.octave2DNormalized(
        xf * params.frequency, yf * params.frequency, params.octaves, params.persistence);
    normal = (normal + 1.f) * 0.5f; // map to [0,1]
    return normal;
}

float PerlinSampler::sample(const glm::vec3& pos) const {
    const float xf = pos.x - origin.x;
    const float yf = pos.y - origin.y;
    const float zf = pos.z - origin.z;
    float normal   = perlin.octave3DNormalized(xf * params.frequency,
                                             yf * params.frequency,
                                             zf * params.frequency,
                                             params.octaves,
                                             params.persistence);
    normal         = (normal + 1.f) * 0.5f; // map to [0,1]
    return normal;
}

} // namespace rand
} // namespace bl
