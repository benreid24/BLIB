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

private:
    float gamma;
    bool dirty;

    friend class Renderer;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

inline float Settings::getGamma() const { return gamma; }

} // namespace rc
} // namespace bl

#endif
