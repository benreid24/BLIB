#include <BLIB/Render/Settings.hpp>

#include <algorithm>
#include <stdexcept>

namespace bl
{
namespace rc
{
namespace
{
constexpr float DefaultGamma                  = 2.2f;
constexpr std::uint32_t DefaultBloomPassCount = 2;
constexpr float DefaultBloomThreshold         = 1.f;
constexpr float DefaultBloomFilters[] = {0.227027f, 0.1945946f, 0.1216216f, 0.054054f, 0.016216f};
} // namespace

Settings::Settings()
: gamma(DefaultGamma)
, exposure(1.f)
, bloomThreshold(DefaultBloomThreshold)
, bloomPasses(DefaultBloomPassCount)
, bloomFilterSize(std::size(DefaultBloomFilters))
, dirty(true) {
    for (unsigned int i = 0; i < std::size(DefaultBloomFilters); ++i) {
        bloomFilters[i] = DefaultBloomFilters[i];
    }
}

Settings& Settings::setGamma(float g) {
    gamma = g;
    dirty = true;
    return *this;
}

Settings& Settings::setExposureFactor(float e) {
    exposure = e;
    dirty    = true;
    return *this;
}

Settings& Settings::setBloomHighlightThreshold(float h) {
    bloomThreshold = h;
    dirty          = true;
    return *this;
}

Settings& Settings::setBloomFilters(std::initializer_list<float> filters) {
    if (filters.size() > bloomFilters.size()) {
        throw std::runtime_error("Too many bloom filters");
    }
    std::copy(filters.begin(), filters.end(), bloomFilters.begin());
    dirty = true;
    return *this;
}

Settings& Settings::setBloomPassCount(std::uint32_t pc) {
    bloomPasses = pc;
    dirty       = true;
    return *this;
}

Settings& Settings::setShadowMapResolution(const VkExtent2D& res) {
    shadowMapResolution = res;
    dirty               = true;
    return *this;
}

} // namespace rc
} // namespace bl
