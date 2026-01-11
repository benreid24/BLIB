#include <BLIB/Render/GraphicsSettings.hpp>

#include <BLIB/Engine/Configuration.hpp>
#include <algorithm>

namespace bl
{
namespace rc
{
namespace
{
constexpr float DefaultGamma                                       = 2.2f;
constexpr bool DefaultHDREnabled                                   = true;
constexpr float DefaultExposureFactor                              = 1.f;
constexpr bool DefaultAutoHDREnabled                               = false;
constexpr GraphicsSettings::AutoHdrSettings DefaultAutoHDRSettings = {0.3f, 0.1f, 5.f, 2.f};
constexpr bool DefaultBloomEnabled                                 = true;
constexpr std::uint32_t DefaultBloomPassCount                      = 2;
constexpr float DefaultBloomThreshold                              = 1.f;
constexpr float DefaultBloomFilters[] = {0.227027f, 0.1945946f, 0.1216216f, 0.054054f, 0.016216f};
constexpr VkExtent2D DefaultShadowMapResolution              = {1024, 1024};
constexpr float DefaultShadowMapDepthBiasConstantFactor      = 1.25f;
constexpr float DefaultShadowMapDepthBiasSlopeFactor         = 1.75f;
constexpr float DefaultShadowMapDepthBiasClamp               = 0.f;
constexpr GraphicsSettings::AntiAliasing DefaultAntiAliasing = GraphicsSettings::AntiAliasing::None;
constexpr GraphicsSettings::SSAO DefaultSSAO                 = GraphicsSettings::SSAO::None;
constexpr float DefaultSSAORadius                            = 1.5f;
constexpr float DefaultSSAOBias                              = 0.025f;
constexpr float DefaultSSAOExponent                          = 5.f;

constexpr const char* GammeKey                     = ".gfx.gamma";
constexpr const char* HDREnabledKey                = ".gfx.hdr_enabled";
constexpr const char* ExposureFactorKey            = ".gfx.exposure_factor";
constexpr const char* AutoHDREnabledKey            = ".gfx.auto_hdr.enabled";
constexpr const char* AutoHDRTargetBrightnessKey   = ".gfx.auto_hdr.target_brightness";
constexpr const char* AutoHDRMinExposureKey        = ".gfx.auto_hdr.min_exposure";
constexpr const char* AutoHDRMaxExposureKey        = ".gfx.auto_hdr.max_exposure";
constexpr const char* AutoHDRConvergeRateKey       = ".gfx.auto_hdr.converge_rate";
constexpr const char* BloomEnabledKey              = ".gfx.bloom.enabled";
constexpr const char* BloomThresholdKey            = ".gfx.bloom.threshold";
constexpr const char* BloomPassesKey               = ".gfx.bloom.passes";
constexpr const char* ShadowMapResolutionWidthKey  = ".gfx.shadow_map.resolution_width";
constexpr const char* ShadowMapResolutionHeightKey = ".gfx.shadow_map.resolution_height";
constexpr const char* ShadowMapDepthBiasConstantFactorKey =
    ".gfx.shadow_map.depth_bias_constant_factor";
constexpr const char* ShadowMapDepthBiasSlopeFactorKey = ".gfx.shadow_map.depth_bias_slope_factor";
constexpr const char* ShadowMapDepthBiasClampKey       = ".gfx.shadow_map.depth_bias_clamp";
constexpr const char* AntiAliasingKey                  = ".gfx.anti_aliasing";
constexpr const char* SSAOKey                          = ".gfx.ssao";
constexpr const char* SSAORadiusKey                    = ".gfx.ssao.radius";
constexpr const char* SSAOBiasKey                      = ".gfx.ssao.bias";
constexpr const char* SSAOExponentKey                  = ".gfx.ssao.exponent";

} // namespace

GraphicsSettings::GraphicsSettings()
: gamma(DefaultGamma)
, hdrEnabled(DefaultHDREnabled)
, exposure(DefaultExposureFactor)
, autoHdrEnabled(DefaultAutoHDREnabled)
, autoHdrSettings(DefaultAutoHDRSettings)
, bloomEnabled(DefaultBloomEnabled)
, bloomThreshold(DefaultBloomThreshold)
, bloomFilters{}
, bloomPasses(DefaultBloomPassCount)
, shadowMapResolution(DefaultShadowMapResolution)
, shadowMapDepthBiasConstantFactor(DefaultShadowMapDepthBiasConstantFactor)
, shadowMapDepthBiasSlopeFactor(DefaultShadowMapDepthBiasSlopeFactor)
, shadowMapDepthBiasClamp(DefaultShadowMapDepthBiasClamp)
, antiAliasing(DefaultAntiAliasing)
, ssao(DefaultSSAO)
, ssaoRadius(DefaultSSAORadius)
, ssaoBias(DefaultSSAOBias)
, ssaoExponent(DefaultSSAOExponent) {
    std::copy(std::begin(DefaultBloomFilters), std::end(DefaultBloomFilters), bloomFilters.begin());
}

GraphicsSettings& GraphicsSettings::fromConfig(const std::string& keyPrefix) {
    using CFG = engine::Configuration;

    gamma    = CFG::getOrDefault<float>(keyPrefix + GammeKey, gamma);
    exposure = CFG::getOrDefault<float>(keyPrefix + ExposureFactorKey, exposure);

    hdrEnabled     = CFG::getOrDefault<bool>(keyPrefix + HDREnabledKey, hdrEnabled);
    autoHdrEnabled = CFG::getOrDefault<bool>(keyPrefix + AutoHDREnabledKey, autoHdrEnabled);
    autoHdrSettings.targetBrightness = CFG::getOrDefault<float>(
        keyPrefix + AutoHDRTargetBrightnessKey, autoHdrSettings.targetBrightness);
    autoHdrSettings.minExposure =
        CFG::getOrDefault<float>(keyPrefix + AutoHDRMinExposureKey, autoHdrSettings.minExposure);
    autoHdrSettings.maxExposure =
        CFG::getOrDefault<float>(keyPrefix + AutoHDRMaxExposureKey, autoHdrSettings.maxExposure);
    autoHdrSettings.convergeRate =
        CFG::getOrDefault<float>(keyPrefix + AutoHDRConvergeRateKey, autoHdrSettings.convergeRate);

    bloomEnabled   = CFG::getOrDefault<bool>(keyPrefix + BloomEnabledKey, bloomEnabled);
    bloomThreshold = CFG::getOrDefault<float>(keyPrefix + BloomThresholdKey, bloomThreshold);
    bloomPasses    = CFG::getOrDefault<std::uint32_t>(keyPrefix + BloomPassesKey, bloomPasses);

    shadowMapResolution.width = CFG::getOrDefault<std::uint32_t>(
        keyPrefix + ShadowMapResolutionWidthKey, shadowMapResolution.width);
    shadowMapResolution.height = CFG::getOrDefault<std::uint32_t>(
        keyPrefix + ShadowMapResolutionHeightKey, shadowMapResolution.height);
    shadowMapDepthBiasConstantFactor = CFG::getOrDefault<float>(
        keyPrefix + ShadowMapDepthBiasConstantFactorKey, shadowMapDepthBiasConstantFactor);
    shadowMapDepthBiasSlopeFactor = CFG::getOrDefault<float>(
        keyPrefix + ShadowMapDepthBiasSlopeFactorKey, shadowMapDepthBiasSlopeFactor);
    shadowMapDepthBiasClamp =
        CFG::getOrDefault<float>(keyPrefix + ShadowMapDepthBiasClampKey, shadowMapDepthBiasClamp);

    antiAliasing = static_cast<AntiAliasing>(
        CFG::getOrDefault<int>(keyPrefix + AntiAliasingKey, static_cast<int>(antiAliasing)));

    ssao = static_cast<SSAO>(CFG::getOrDefault<int>(keyPrefix + SSAOKey, static_cast<int>(ssao)));
    ssaoRadius   = CFG::getOrDefault<float>(keyPrefix + SSAORadiusKey, ssaoRadius);
    ssaoBias     = CFG::getOrDefault<float>(keyPrefix + SSAOBiasKey, ssaoBias);
    ssaoExponent = CFG::getOrDefault<float>(keyPrefix + SSAOExponentKey, ssaoExponent);

    return *this;
}

void GraphicsSettings::syncToConfig(const std::string& keyPrefix) const {
    using CFG = engine::Configuration;

    CFG::set<float>(keyPrefix + GammeKey, gamma);
    CFG::set<float>(keyPrefix + ExposureFactorKey, exposure);

    CFG::set<bool>(keyPrefix + HDREnabledKey, hdrEnabled);
    CFG::set<float>(keyPrefix + AutoHDRTargetBrightnessKey, autoHdrSettings.targetBrightness);
    CFG::set<float>(keyPrefix + AutoHDRMinExposureKey, autoHdrSettings.minExposure);
    CFG::set<float>(keyPrefix + AutoHDRMaxExposureKey, autoHdrSettings.maxExposure);

    CFG::set<bool>(keyPrefix + BloomEnabledKey, bloomEnabled);
    CFG::set<float>(keyPrefix + BloomThresholdKey, bloomThreshold);
    CFG::set<std::uint32_t>(keyPrefix + BloomPassesKey, bloomPasses);

    CFG::set<std::uint32_t>(keyPrefix + ShadowMapResolutionWidthKey, shadowMapResolution.width);
    CFG::set<std::uint32_t>(keyPrefix + ShadowMapResolutionHeightKey, shadowMapResolution.height);
    CFG::set<float>(keyPrefix + ShadowMapDepthBiasConstantFactorKey,
                    shadowMapDepthBiasConstantFactor);
    CFG::set<float>(keyPrefix + ShadowMapDepthBiasSlopeFactorKey, shadowMapDepthBiasSlopeFactor);
    CFG::set<float>(keyPrefix + ShadowMapDepthBiasClampKey, shadowMapDepthBiasClamp);

    CFG::set<int>(keyPrefix + AntiAliasingKey, static_cast<int>(antiAliasing));

    CFG::set<int>(keyPrefix + SSAOKey, static_cast<int>(ssao));
    CFG::set<float>(keyPrefix + SSAORadiusKey, ssaoRadius);
    CFG::set<float>(keyPrefix + SSAOBiasKey, ssaoBias);
    CFG::set<float>(keyPrefix + SSAOExponentKey, ssaoExponent);
}

} // namespace rc
} // namespace bl