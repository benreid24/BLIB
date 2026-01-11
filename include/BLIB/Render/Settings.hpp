#ifndef BLIB_RENDER_SETTINGS_HPP
#define BLIB_RENDER_SETTINGS_HPP

#include <BLIB/Render/Config/Limits.hpp>
#include <BLIB/Render/CreationSettings.hpp>
#include <BLIB/Render/Events/SettingsChanged.hpp>
#include <BLIB/Render/GraphicsSettings.hpp>
#include <BLIB/Signals/Emitter.hpp>
#include <BLIB/Vulkan.hpp>
#include <array>
#include <initializer_list>

namespace bl
{
namespace rc
{
class Renderer;

/**
 * @brief Basic class containing the renderer graphics settings
 *
 * @ingroup Renderer
 */
class Settings {
public:
    using AntiAliasing    = GraphicsSettings::AntiAliasing;
    using SSAO            = GraphicsSettings::SSAO;
    using AutoHdrSettings = GraphicsSettings::AutoHdrSettings;

    /**
     * @brief Initializes the settings to sane defaults
     *
     * @param owner The renderer that owns these settings
     * @param settings The settings to initialize from
     */
    Settings(Renderer& owner, const CreationSettings& settings);

    /**
     * @brief Returns the gamma value to use for rendering
     */
    float getGamma() const;

    /**
     * @brief Set the gamma value to use when rendering
     *
     * @param g The new gamma value
     * @return A reference to this object
     */
    Settings& setGamma(float g);

    /**
     * @brief Returns the exposure factor used for HDR tone mapping
     */
    float getExposureFactor() const;

    /**
     * @brief Sets the exposure factor used for HDR tone mapping
     *
     * @param e The exposure factor to use
     * @return A reference to this object
     */
    Settings& setExposureFactor(float e);

    /**
     * @brief Returns whether HDR rendering is enabled
     */
    bool getHDREnabled() const;

    /**
     * @brief Sets whether HDR rendering is enabled
     *
     * @param enable True to enable HDR rendering and tone mapping, false to disable
     * @return A reference to this object
     */
    Settings& setHDREnabled(bool enable);

    /**
     * @brief Sets the Auto HDR settings. Only used if both HDR and auto-hdr are enabled
     *
     * @param settings The AutoHdrSettings object containing the desired HDR configuration
     * @return A reference to this object
     */
    Settings& setAutoHDRSettings(const AutoHdrSettings& settings);

    /**
     * @brief Returns the auto HDR settings of the renderer
     */
    const AutoHdrSettings& getAutoHDRSettings() const;

    /**
     * @brief Sets whether auto-hdr exposure is enabled
     *
     * @param enable True to enable automatic exposure adjustment, false to disable
     * @return A reference to this object
     */
    Settings& setAutoHDREnabled(bool enable);

    /**
     * @brief Returns whether auto-hdr exposure adjustment is enabled
     */
    bool getAutoHDREnabled() const;

    /**
     * @brief Returns whether bloom is enabled
     */
    bool getBloomEnabled() const;

    /**
     * @brief Sets whether bloom is enabled
     *
     * @param enable True to enable bloom, false to disable
     * @return A reference to this object
     */
    Settings& setBloomEnabled(bool enable);

    /**
     * @brief Returns the light level that bloom will be applied to
     */
    float getBloomHighlightThreshold() const;

    /**
     * @brief Sets the light level that bloom will be applied to
     *
     * @param t The light level that bloom starts at
     * @return A reference to this object
     */
    Settings& setBloomHighlightThreshold(float t);

    /**
     * @brief Returns the number of blue passes to use for bloom
     */
    std::uint32_t getBloomPassCount() const;

    /**
     * @brief Sets the number of bloom passes to use
     *
     * @param p The number of bloom passes to use
     * @return A reference to this object
     */
    Settings& setBloomPassCount(std::uint32_t p);

    /**
     * @brief Returns the number size of the bloom filter in texels
     */
    std::uint32_t getBloomFilterSize() const;

    /**
     * @brief Returns the bloom filter weights
     */
    const std::array<float, cfg::Limits::MaxBloomFilterSize>& getBloomFilters() const;

    /**
     * @brief Sets the bloom filter weights
     *
     * @param filters The filter weights to use
     * @return A reference to this object
     */
    Settings& setBloomFilters(std::initializer_list<float> filters);

    /**
     * @brief Sets the resolution to render shadow maps at
     *
     * @param resolution The shadow map resolution to use
     * @return A reference to this object
     */
    Settings& setShadowMapResolution(const VkExtent2D& resolution);

    /**
     * @brief Returns the resolution to render shadow maps at
     */
    const VkExtent2D& getShadowMapResolution() const;

    /**
     * @brief Sets the depth bias parameters for shadow maps
     *
     * @param constantFactor Constant bias factor
     * @param slope Factor to apply to fragment slope
     * @param clamp Min or max depth bias
     */
    Settings& setShadowMapDepthBias(float constantFactor, float slope, float clamp);

    /**
     * @brief Returns the constant factor for shadow map depth bias
     */
    float getShadowMapDepthBiasConstantFactor() const;

    /**
     * @brief Returns the slope factor for shadow map depth bias
     */
    float getShadowMapDepthBiasSlopeFactor() const;

    /**
     * @brief Returns the clamp value for shadow map depth bias
     */
    float getShadowMapDepthBiasClamp() const;

    /**
     * @brief Returns the anti-aliasing mode used by the renderer
     */
    AntiAliasing getAntiAliasing() const;

    /**
     * @brief Sets the anti-aliasing mode used by the renderer
     *
     * @param aa The anti-aliasing mode to use
     * @return A reference to this object
     */
    Settings& setAntiAliasing(AntiAliasing aa);

    /**
     * @brief Returns the sample count used for MSAA
     */
    VkSampleCountFlagBits getMSAASampleCount() const;

    /**
     * @brief Return the sample count used for MSAA as an unsigned int
     */
    unsigned int getMSAASampleCountAsInt() const;

    /**
     * @brief Modifies the SSAO setting
     *
     * @param ssao The new SSAO setting
     * @return A reference to this object
     */
    Settings& setSSAO(SSAO ssao);

    /**
     * @brief Sets some parameters of the SSAO algorithm
     *
     * @param radius The radius to sample for occlusion detection, in world space
     * @param bias A depth bias to add to the sampled geometry, in world space
     * @param exponent The exponent to raise the SSAO factor to
     * @return A reference to this object
     */
    Settings& setSSAOParams(float radius, float bias, float exponent);

    /**
     * @brief Returns the current SSAO setting
     */
    SSAO getSSAO() const;

    /**
     * @brief Returns the radius used by the SSAO algorithm in world units
     */
    float getSSAORadius() const;

    /**
     * @brief Returns the depth bias used by the SSAO algorithm in world units
     */
    float getSSAOBias() const;

    /**
     * @brief Returns the exponent that the SSAO factor is raised to
     */
    float getSSAOExponent() const;

    /**
     * @brief Returns the window settings used by the renderer
     */
    const WindowSettings& getWindowSettings() const;

    /**
     * @brief Returns the window settings used by the renderer
     */
    WindowSettings& getWindowSettings();

private:
    Renderer& owner;
    WindowSettings windowSettings;
    GraphicsSettings graphicsSettings;

    sig::Emitter<event::SettingsChanged> emitter;
    bool dirty;

    friend class Renderer;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

inline float Settings::getGamma() const { return graphicsSettings.gamma; }

inline float Settings::getExposureFactor() const { return graphicsSettings.exposure; }

inline bool Settings::getHDREnabled() const { return graphicsSettings.hdrEnabled; }

inline const Settings::AutoHdrSettings& Settings::getAutoHDRSettings() const {
    return graphicsSettings.autoHdrSettings;
}

inline bool Settings::getAutoHDREnabled() const { return graphicsSettings.autoHdrEnabled; }

inline bool Settings::getBloomEnabled() const { return graphicsSettings.bloomEnabled; }

inline float Settings::getBloomHighlightThreshold() const {
    return graphicsSettings.bloomThreshold;
}

inline std::uint32_t Settings::getBloomPassCount() const { return graphicsSettings.bloomPasses; }

inline std::uint32_t Settings::getBloomFilterSize() const {
    return graphicsSettings.bloomFilterSize;
}

inline const std::array<float, cfg::Limits::MaxBloomFilterSize>& Settings::getBloomFilters() const {
    return graphicsSettings.bloomFilters;
}

inline const VkExtent2D& Settings::getShadowMapResolution() const {
    return graphicsSettings.shadowMapResolution;
}

inline float Settings::getShadowMapDepthBiasConstantFactor() const {
    return graphicsSettings.shadowMapDepthBiasConstantFactor;
}

inline float Settings::getShadowMapDepthBiasSlopeFactor() const {
    return graphicsSettings.shadowMapDepthBiasSlopeFactor;
}

inline float Settings::getShadowMapDepthBiasClamp() const {
    return graphicsSettings.shadowMapDepthBiasClamp;
}

inline Settings::AntiAliasing Settings::getAntiAliasing() const {
    return graphicsSettings.antiAliasing;
}

inline Settings::SSAO Settings::getSSAO() const { return graphicsSettings.ssao; }

inline float Settings::getSSAORadius() const { return graphicsSettings.ssaoRadius; }

inline float Settings::getSSAOBias() const { return graphicsSettings.ssaoBias; }

inline float Settings::getSSAOExponent() const { return graphicsSettings.ssaoExponent; }

inline const WindowSettings& Settings::getWindowSettings() const { return windowSettings; }

inline WindowSettings& Settings::getWindowSettings() { return windowSettings; }

} // namespace rc
} // namespace bl

#endif
