#include <BLIB/Random/Seed.hpp>

namespace bl
{
namespace rand
{
Seed::Seed()
: Seed(std::random_device{}()) {}

Seed::Seed(std::uint64_t seed)
: seed(seed)
, rng(seed) {}

void Seed::reseed(std::uint64_t seed) {
    this->seed = seed;
    rng.seed(seed);
}

} // namespace rand
} // namespace bl
