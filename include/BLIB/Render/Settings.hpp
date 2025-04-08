#ifndef BLIB_RENDER_SETTINGS_HPP
#define BLIB_RENDER_SETTINGS_HPP

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

private:
    float gamma;
    float exposure;
    bool dirty;

    friend class Renderer;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

inline float Settings::getGamma() const { return gamma; }

inline float Settings::getExposureFactor() const { return exposure; }

} // namespace rc
} // namespace bl

#endif
