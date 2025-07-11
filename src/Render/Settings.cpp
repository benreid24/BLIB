#include <BLIB/Render/Settings.hpp>

#include <BLIB/Events.hpp>
#include <BLIB/Render/Events/SettingsChanged.hpp>
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
constexpr VkExtent2D DefaultShadowMapResolution         = {1024, 1024};
constexpr float DefaultShadowMapDepthBiasConstantFactor = 1.25f;
constexpr float DefaultShadowMapDepthBiasSlopeFactor    = 1.75f;
constexpr float DefaultShadowMapDepthBiasClamp          = 0.f;
constexpr Settings::AntiAliasing DefaultAntiAliasing    = Settings::AntiAliasing::None;

using Changed = event::SettingsChanged;
using Setting = Changed::Setting;
} // namespace

Settings::Settings(Renderer& owner)
: owner(owner)
, gamma(DefaultGamma)
, exposure(1.f)
, bloomThreshold(DefaultBloomThreshold)
, bloomPasses(DefaultBloomPassCount)
, bloomFilterSize(std::size(DefaultBloomFilters))
, shadowMapResolution(DefaultShadowMapResolution)
, shadowMapDepthBiasConstantFactor(DefaultShadowMapDepthBiasConstantFactor)
, shadowMapDepthBiasSlopeFactor(DefaultShadowMapDepthBiasSlopeFactor)
, shadowMapDepthBiasClamp(DefaultShadowMapDepthBiasClamp)
, antiAliasing(DefaultAntiAliasing)
, dirty(true) {
    for (unsigned int i = 0; i < std::size(DefaultBloomFilters); ++i) {
        bloomFilters[i] = DefaultBloomFilters[i];
    }
}

Settings& Settings::setGamma(float g) {
    gamma = g;
    dirty = true;
    bl::event::Dispatcher::dispatch<Changed>({owner, *this, Setting::Gamma});
    return *this;
}

Settings& Settings::setExposureFactor(float e) {
    exposure = e;
    dirty    = true;
    bl::event::Dispatcher::dispatch<Changed>({owner, *this, Setting::ExposureFactor});
    return *this;
}

Settings& Settings::setBloomHighlightThreshold(float h) {
    bloomThreshold = h;
    dirty          = true;
    bl::event::Dispatcher::dispatch<Changed>({owner, *this, Setting::BloomHighlightThreshold});
    return *this;
}

Settings& Settings::setBloomFilters(std::initializer_list<float> filters) {
    if (filters.size() > bloomFilters.size()) {
        throw std::runtime_error("Too many bloom filters");
    }
    std::copy(filters.begin(), filters.end(), bloomFilters.begin());
    dirty = true;
    bl::event::Dispatcher::dispatch<Changed>({owner, *this, Setting::BloomFilters});
    return *this;
}

Settings& Settings::setBloomPassCount(std::uint32_t pc) {
    bloomPasses = pc;
    dirty       = true;
    bl::event::Dispatcher::dispatch<Changed>({owner, *this, Setting::BloomPassCount});
    return *this;
}

Settings& Settings::setShadowMapResolution(const VkExtent2D& res) {
    shadowMapResolution = res;
    dirty               = true;
    bl::event::Dispatcher::dispatch<Changed>({owner, *this, Setting::ShadowMapResolution});
    return *this;
}

Settings& Settings::setShadowMapDepthBias(float constantFactor, float slopeFactor, float clamp) {
    shadowMapDepthBiasConstantFactor = constantFactor;
    shadowMapDepthBiasSlopeFactor    = slopeFactor;
    shadowMapDepthBiasClamp          = clamp;
    bl::event::Dispatcher::dispatch<Changed>({owner, *this, Setting::ShadowMapDepthBias});
    return *this;
}

Settings& Settings::setAntiAliasing(AntiAliasing aa) {
    antiAliasing = aa;
    dirty        = true;
    bl::event::Dispatcher::dispatch<Changed>({owner, *this, Setting::AntiAliasing});
    return *this;
}

VkSampleCountFlagBits Settings::getMSAASampleCount() const {
    return static_cast<VkSampleCountFlagBits>(antiAliasing);
}

} // namespace rc
} // namespace bl
