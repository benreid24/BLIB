#ifndef BLIB_RENDER_STOREDSETTINGS_HPP
#define BLIB_RENDER_STOREDSETTINGS_HPP

#include <BLIB/Render/Config/Limits.hpp>
#include <BLIB/Vulkan.hpp>
#include <array>
#include <string>

namespace bl
{
namespace rc
{
/**
 * @brief POD containing non-ephemeral renderer settings
 *
 * @ingroup Renderer
 */
struct GraphicsSettings {
    /// The anti-aliasing modes supported by the renderer
    enum struct AntiAliasing {
        None    = VK_SAMPLE_COUNT_1_BIT,
        MSAA2x  = VK_SAMPLE_COUNT_2_BIT,
        MSAA4x  = VK_SAMPLE_COUNT_4_BIT,
        MSAA8x  = VK_SAMPLE_COUNT_8_BIT,
        MSAA16x = VK_SAMPLE_COUNT_16_BIT,
        MSAA32x = VK_SAMPLE_COUNT_32_BIT,
        MSAA64x = VK_SAMPLE_COUNT_64_BIT
    };

    /// Presets for SSAO
    enum struct SSAO { None, Low, Medium, High, Ultra };

    /// Settings that control HDR tone mapping auto exposure
    struct AutoHdrSettings {
        /// The starting exposure value
        float targetBrightness;

        /// The minimum exposure value
        float minExposure;

        /// The maximum exposure value
        float maxExposure;

        /// The normalized rate at which exposure converges to the target
        float convergeRate;
    };

    /**
     * @brief Creates the default settings
     */
    GraphicsSettings();

    float gamma;
    bool hdrEnabled;
    float exposure;
    bool autoHdrEnabled;
    AutoHdrSettings autoHdrSettings;
    bool bloomEnabled;
    float bloomThreshold;
    std::array<float, cfg::Limits::MaxBloomFilterSize> bloomFilters;
    std::uint32_t bloomFilterSize;
    std::uint32_t bloomPasses;
    VkExtent2D shadowMapResolution;
    float shadowMapDepthBiasConstantFactor;
    float shadowMapDepthBiasSlopeFactor;
    float shadowMapDepthBiasClamp;
    AntiAliasing antiAliasing;
    SSAO ssao;
    float ssaoRadius;
    float ssaoBias;
    float ssaoExponent;

    /**
     * @brief Loads the renderer settings from the global engine config
     *
     * @param keyPrefix The prefix to use when loading from the config
     * @return A reference to this object
     */
    GraphicsSettings& fromConfig(const std::string& keyPrefix);

    /**
     * @brief Saves the settings to the global engine config
     *
     * @param keyPrefix The prefix to use when saving to the config
     */
    void syncToConfig(const std::string& keyPrefix) const;
};

} // namespace rc
} // namespace bl

#endif
