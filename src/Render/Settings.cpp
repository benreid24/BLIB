#include <BLIB/Render/Settings.hpp>

#include <BLIB/Render/Renderer.hpp>
#include <algorithm>
#include <stdexcept>

namespace bl
{
namespace rc
{
namespace
{
constexpr float DefaultGamma                               = 2.2f;
constexpr bool DefaultHDREnabled                           = true;
constexpr float DefaultExposureFactor                      = 1.f;
constexpr bool DefaultAutoHDREnabled                       = false;
constexpr Settings::AutoHdrSettings DefaultAutoHDRSettings = {0.3f, 0.1f, 5.f, 2.f};
constexpr bool DefaultBloomEnabled                         = true;
constexpr std::uint32_t DefaultBloomPassCount              = 2;
constexpr float DefaultBloomThreshold                      = 1.f;
constexpr float DefaultBloomFilters[] = {0.227027f, 0.1945946f, 0.1216216f, 0.054054f, 0.016216f};
constexpr VkExtent2D DefaultShadowMapResolution         = {1024, 1024};
constexpr float DefaultShadowMapDepthBiasConstantFactor = 1.25f;
constexpr float DefaultShadowMapDepthBiasSlopeFactor    = 1.75f;
constexpr float DefaultShadowMapDepthBiasClamp          = 0.f;
constexpr Settings::AntiAliasing DefaultAntiAliasing    = Settings::AntiAliasing::None;
constexpr Settings::SSAO DefaultSSAO                    = Settings::SSAO::None;
constexpr float DefaultSSAORadius                       = 1.5f;
constexpr float DefaultSSAOBias                         = 0.025f;
constexpr float DefaultSSAOExponent                     = 5.f;

using Changed = event::SettingsChanged;
using Setting = Changed::Setting;
} // namespace

Settings::Settings(Renderer& owner)
: owner(owner)
, gamma(DefaultGamma)
, hdrEnabled(DefaultHDREnabled)
, exposure(DefaultExposureFactor)
, autoHdrEnabled(DefaultAutoHDREnabled)
, autoHdrSettings(DefaultAutoHDRSettings)
, bloomEnabled(DefaultBloomEnabled)
, bloomThreshold(DefaultBloomThreshold)
, bloomPasses(DefaultBloomPassCount)
, bloomFilterSize(std::size(DefaultBloomFilters))
, shadowMapResolution(DefaultShadowMapResolution)
, shadowMapDepthBiasConstantFactor(DefaultShadowMapDepthBiasConstantFactor)
, shadowMapDepthBiasSlopeFactor(DefaultShadowMapDepthBiasSlopeFactor)
, shadowMapDepthBiasClamp(DefaultShadowMapDepthBiasClamp)
, antiAliasing(DefaultAntiAliasing)
, ssao(DefaultSSAO)
, ssaoRadius(DefaultSSAORadius)
, ssaoBias(DefaultSSAOBias)
, ssaoExponent(DefaultSSAOExponent)
, dirty(true) {
    for (unsigned int i = 0; i < std::size(DefaultBloomFilters); ++i) {
        bloomFilters[i] = DefaultBloomFilters[i];
    }
}

Settings& Settings::setGamma(float g) {
    gamma = g;
    dirty = true;
    emitter.emit<Changed>({owner, *this, Setting::Gamma});
    return *this;
}

Settings& Settings::setExposureFactor(float e) {
    exposure = e;
    dirty    = true;
    emitter.emit<Changed>({owner, *this, Setting::ExposureFactor});
    return *this;
}

Settings& Settings::setHDREnabled(bool e) {
    if (hdrEnabled != e) {
        hdrEnabled = e;
        owner.vulkanState().textureFormatManager.setFormat(
            vk::SemanticTextureFormat::Color,
            hdrEnabled ? vk::CommonTextureFormats::HDRColor : vk::CommonTextureFormats::SRGBA32Bit);
        dirty = true;
        emitter.emit<Changed>({owner, *this, Setting::HDR});
    }
    return *this;
}

Settings& Settings::setAutoHDRSettings(const AutoHdrSettings& settings) {
    autoHdrSettings = settings;
    emitter.emit<Changed>({owner, *this, Setting::AutoHDRParams});
    return *this;
}

Settings& Settings::setAutoHDREnabled(bool e) {
    if (autoHdrEnabled != e) {
        autoHdrEnabled = e;
        dirty          = true;
        emitter.emit<Changed>({owner, *this, Setting::AutoHDR});
    }
    return *this;
}

Settings& Settings::setBloomEnabled(bool e) {
    if (bloomEnabled != e) {
        bloomEnabled = e;
        dirty        = true;
        emitter.emit<Changed>({owner, *this, Setting::BloomEnabled});
    }
    return *this;
}

Settings& Settings::setBloomHighlightThreshold(float h) {
    bloomThreshold = h;
    dirty          = true;
    emitter.emit<Changed>({owner, *this, Setting::BloomHighlightThreshold});
    return *this;
}

Settings& Settings::setBloomFilters(std::initializer_list<float> filters) {
    if (filters.size() > bloomFilters.size()) {
        throw std::runtime_error("Too many bloom filters");
    }
    std::copy(filters.begin(), filters.end(), bloomFilters.begin());
    dirty = true;
    emitter.emit<Changed>({owner, *this, Setting::BloomFilters});
    return *this;
}

Settings& Settings::setBloomPassCount(std::uint32_t pc) {
    bloomPasses = pc;
    dirty       = true;
    emitter.emit<Changed>({owner, *this, Setting::BloomPassCount});
    return *this;
}

Settings& Settings::setShadowMapResolution(const VkExtent2D& res) {
    shadowMapResolution = res;
    dirty               = true;
    emitter.emit<Changed>({owner, *this, Setting::ShadowMapResolution});
    return *this;
}

Settings& Settings::setShadowMapDepthBias(float constantFactor, float slopeFactor, float clamp) {
    shadowMapDepthBiasConstantFactor = constantFactor;
    shadowMapDepthBiasSlopeFactor    = slopeFactor;
    shadowMapDepthBiasClamp          = clamp;
    emitter.emit<Changed>({owner, *this, Setting::ShadowMapDepthBias});
    return *this;
}

Settings& Settings::setAntiAliasing(AntiAliasing aa) {
    antiAliasing = aa;
    dirty        = true;
    emitter.emit<Changed>({owner, *this, Setting::AntiAliasing});
    return *this;
}

Settings& Settings::setSSAO(SSAO ao) {
    ssao  = ao;
    dirty = true;
    emitter.emit<Changed>({owner, *this, Setting::SSAO});
    return *this;
}

Settings& Settings::setSSAOParams(float radius, float bias, float exp) {
    ssaoRadius   = radius;
    ssaoBias     = bias;
    ssaoExponent = exp;
    emitter.emit<Changed>({owner, *this, Setting::SSAOParams});
    return *this;
}

VkSampleCountFlagBits Settings::getMSAASampleCount() const {
    return static_cast<VkSampleCountFlagBits>(antiAliasing);
}

unsigned int Settings::getMSAASampleCountAsInt() const {
    switch (antiAliasing) {
    case AntiAliasing::MSAA2x:
        return 2;
    case AntiAliasing::MSAA4x:
        return 4;
    case AntiAliasing::MSAA8x:
        return 8;
    case AntiAliasing::MSAA16x:
        return 16;
    case AntiAliasing::MSAA32x:
        return 32;
    case AntiAliasing::MSAA64x:
        return 64;

    default:
    case AntiAliasing::None:
        return 1;
    }
}

} // namespace rc
} // namespace bl
