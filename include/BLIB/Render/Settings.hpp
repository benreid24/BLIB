#ifndef BLIB_RENDER_SETTINGS_HPP
#define BLIB_RENDER_SETTINGS_HPP

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
    static constexpr std::uint32_t MaxBloomFilterSize = 20;

    /**
     * @brief Initializes the settings to sane defaults
     */
    Settings();

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
    const std::array<float, MaxBloomFilterSize>& getBloomFilters() const;

    /**
     * @brief Sets the bloom filter weights
     *
     * @param filters The filter weights to use
     * @return A reference to this object
     */
    Settings& setBloomFilters(std::initializer_list<float> filters);

    /**
     * @brief Sets the resoltuion to render shadow maps at
     *
     * @param resolution The shadow map resolution to use
     * @return A reference to this object
     */
    Settings& setShadowMapResolution(const VkExtent2D& resolution);

    /**
     * @brief Returns the resolution to render shadow maps at
     */
    const VkExtent2D& getShadowMapResolution() const;

private:
    float gamma;
    float exposure;
    float bloomThreshold;
    std::array<float, MaxBloomFilterSize> bloomFilters;
    std::uint32_t bloomFilterSize;
    std::uint32_t bloomPasses;
    VkExtent2D shadowMapResolution;
    bool dirty;

    friend class Renderer;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

inline float Settings::getGamma() const { return gamma; }

inline float Settings::getExposureFactor() const { return exposure; }

inline float Settings::getBloomHighlightThreshold() const { return bloomThreshold; }

inline std::uint32_t Settings::getBloomPassCount() const { return bloomPasses; }

inline std::uint32_t Settings::getBloomFilterSize() const { return bloomFilterSize; }

inline const std::array<float, Settings::MaxBloomFilterSize>& Settings::getBloomFilters() const {
    return bloomFilters;
}

inline const VkExtent2D& Settings::getShadowMapResolution() const { return shadowMapResolution; }

} // namespace rc
} // namespace bl

#endif
