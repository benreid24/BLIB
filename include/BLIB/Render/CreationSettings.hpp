#ifndef BLIB_RENDER_CREATIONSETTINGS_HPP
#define BLIB_RENDER_CREATIONSETTINGS_HPP

#include <BLIB/Render/GraphicsSettings.hpp>
#include <BLIB/Render/WindowSettings.hpp>

namespace bl
{
namespace rc
{
/**
 * @brief Settings object used when creating a Renderer
 *
 * @ingroup Renderer
 */
class CreationSettings {
public:
    /**
     * @brief Initializes with the default settings
     */
    CreationSettings() = default;

    /**
     * @brief Set the window settings to use when creating the renderer window
     *
     * @param ws The window settings to use
     * @return A reference to this object
     */
    CreationSettings& withWindowSettings(const WindowSettings& ws) {
        windowSettings = ws;
        return *this;
    }

    /**
     * @brief Set the graphics settings to use for the renderer
     *
     * @param gs The graphics settings to use
     * @return A reference to this object
     */
    CreationSettings& withGraphicsSettings(const GraphicsSettings& gs) {
        graphicsSettings = gs;
        return *this;
    }

    /**
     * @brief Returns the window settings to use when creating the renderer window
     */
    const WindowSettings& getWindowSettings() const { return windowSettings; }

    /**
     * @brief Returns the graphics settings to use for the renderer
     */
    const GraphicsSettings& getGraphicsSettings() const { return graphicsSettings; }

    /**
     * @brief Writes the contained settings to the engine config store
     *
     * @param keyPrefix The prefix to use when saving to the config
     */
    void syncToConfig(const std::string& keyPrefix) const {
        windowSettings.syncToConfig(keyPrefix);
        graphicsSettings.syncToConfig(keyPrefix);
    }

    /**
     * @brief Reads the contained settings from the engine config store
     *
     * @param keyPrefix The prefix to use when loading from the config
     */
    void fromConfig(const std::string& keyPrefix) {
        windowSettings.fromConfig(keyPrefix);
        graphicsSettings.fromConfig(keyPrefix);
    }

private:
    WindowSettings windowSettings;
    GraphicsSettings graphicsSettings;
};

} // namespace rc
} // namespace bl

#endif
