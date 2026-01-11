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

using Changed = event::SettingsChanged;
using Setting = Changed::Setting;
} // namespace

Settings::Settings(Renderer& owner, const CreationSettings& src)
: owner(owner)
, windowSettings(src.getWindowSettings())
, graphicsSettings(src.getGraphicsSettings())
, dirty(true) {}

Settings& Settings::setGamma(float g) {
    graphicsSettings.gamma = g;
    dirty                  = true;
    emitter.emit<Changed>({owner, *this, Setting::Gamma});
    return *this;
}

Settings& Settings::setExposureFactor(float e) {
    graphicsSettings.exposure = e;
    dirty                     = true;
    emitter.emit<Changed>({owner, *this, Setting::ExposureFactor});
    return *this;
}

Settings& Settings::setHDREnabled(bool e) {
    if (graphicsSettings.hdrEnabled != e) {
        graphicsSettings.hdrEnabled = e;
        owner.getTextureFormatManager().setFormat(vk::SemanticTextureFormat::Color,
                                                  graphicsSettings.hdrEnabled ?
                                                      vk::CommonTextureFormats::HDRColor :
                                                      vk::CommonTextureFormats::SRGBA32Bit);
        dirty = true;
        emitter.emit<Changed>({owner, *this, Setting::HDR});
    }
    return *this;
}

Settings& Settings::setAutoHDRSettings(const AutoHdrSettings& settings) {
    graphicsSettings.autoHdrSettings = settings;
    emitter.emit<Changed>({owner, *this, Setting::AutoHDRParams});
    return *this;
}

Settings& Settings::setAutoHDREnabled(bool e) {
    if (graphicsSettings.autoHdrEnabled != e) {
        graphicsSettings.autoHdrEnabled = e;
        dirty                           = true;
        emitter.emit<Changed>({owner, *this, Setting::AutoHDR});
    }
    return *this;
}

Settings& Settings::setBloomEnabled(bool e) {
    if (graphicsSettings.bloomEnabled != e) {
        graphicsSettings.bloomEnabled = e;
        dirty                         = true;
        emitter.emit<Changed>({owner, *this, Setting::BloomEnabled});
    }
    return *this;
}

Settings& Settings::setBloomHighlightThreshold(float h) {
    graphicsSettings.bloomThreshold = h;
    dirty                           = true;
    emitter.emit<Changed>({owner, *this, Setting::BloomHighlightThreshold});
    return *this;
}

Settings& Settings::setBloomFilters(std::initializer_list<float> filters) {
    if (filters.size() > graphicsSettings.bloomFilters.size()) {
        throw std::runtime_error("Too many bloom filters");
    }
    std::copy(filters.begin(), filters.end(), graphicsSettings.bloomFilters.begin());
    dirty = true;
    emitter.emit<Changed>({owner, *this, Setting::BloomFilters});
    return *this;
}

Settings& Settings::setBloomPassCount(std::uint32_t pc) {
    graphicsSettings.bloomPasses = pc;
    dirty                        = true;
    emitter.emit<Changed>({owner, *this, Setting::BloomPassCount});
    return *this;
}

Settings& Settings::setShadowMapResolution(const VkExtent2D& res) {
    graphicsSettings.shadowMapResolution = res;
    dirty                                = true;
    emitter.emit<Changed>({owner, *this, Setting::ShadowMapResolution});
    return *this;
}

Settings& Settings::setShadowMapDepthBias(float constantFactor, float slopeFactor, float clamp) {
    graphicsSettings.shadowMapDepthBiasConstantFactor = constantFactor;
    graphicsSettings.shadowMapDepthBiasSlopeFactor    = slopeFactor;
    graphicsSettings.shadowMapDepthBiasClamp          = clamp;
    emitter.emit<Changed>({owner, *this, Setting::ShadowMapDepthBias});
    return *this;
}

Settings& Settings::setAntiAliasing(AntiAliasing aa) {
    graphicsSettings.antiAliasing = aa;
    dirty                         = true;
    emitter.emit<Changed>({owner, *this, Setting::AntiAliasing});
    return *this;
}

Settings& Settings::setSSAO(SSAO ao) {
    graphicsSettings.ssao = ao;
    dirty                 = true;
    emitter.emit<Changed>({owner, *this, Setting::SSAO});
    return *this;
}

Settings& Settings::setSSAOParams(float radius, float bias, float exp) {
    graphicsSettings.ssaoRadius   = radius;
    graphicsSettings.ssaoBias     = bias;
    graphicsSettings.ssaoExponent = exp;
    emitter.emit<Changed>({owner, *this, Setting::SSAOParams});
    return *this;
}

VkSampleCountFlagBits Settings::getMSAASampleCount() const {
    return static_cast<VkSampleCountFlagBits>(graphicsSettings.antiAliasing);
}

unsigned int Settings::getMSAASampleCountAsInt() const {
    switch (graphicsSettings.antiAliasing) {
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
