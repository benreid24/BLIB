#ifndef BLIB_RENDER_EVENTS_SETTINGSCHANGED_HPP
#define BLIB_RENDER_EVENTS_SETTINGSCHANGED_HPP

namespace bl
{
namespace rc
{
class Renderer;
class Settings;
namespace event
{
/**
 * @brief Fired when renderer settings are changed
 *
 * @ingroup Renderer
 */
struct SettingsChanged {
    enum Setting {
        Gamma,
        HDR,
        ExposureFactor,
        AutoHDR,
        AutoHDRParams,
        BloomEnabled,
        BloomHighlightThreshold,
        BloomPassCount,
        BloomFilters,
        ShadowMapResolution,
        ShadowMapDepthBias,
        AntiAliasing,
        SSAO,
        SSAOParams
    };

    /// The renderer instance
    Renderer& renderer;

    /// The renderer settings
    const Settings& settings;

    /// The setting that was changed
    const Setting setting;

    /**
     * @brief Constructs the event with the given settings
     *
     * @param renderer The renderer that the settings were changed for
     * @param settings The settings that were changed
     * @param setting The setting that was changed
     */
    SettingsChanged(Renderer& renderer, const Settings& settings, Setting setting)
    : renderer(renderer)
    , settings(settings)
    , setting(setting) {}
};

} // namespace event
} // namespace rc
} // namespace bl

#endif