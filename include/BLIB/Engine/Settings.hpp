#ifndef BLIB_ENGINE_SETTINGS_HPP
#define BLIB_ENGINE_SETTINGS_HPP

#include <BLIB/Render/CreationSettings.hpp>
#include <SFML/Graphics/Image.hpp>
#include <SFML/Window.hpp>
#include <optional>
#include <string>

namespace bl
{
namespace engine
{
/**
 * @brief Collection of settings to create an Engine with, organized as a builder
 *
 * @ingroup Engine
 */
class Settings {
public:
    static constexpr float DefaultUpdateInterval       = 1.f / 120.f;
    static constexpr float DefaultMaximumFramerate     = 0.f;
    static constexpr bool DefaultAllowVariableTimestep = true;
    static constexpr bool DefaultCreateWindow          = true;
#ifdef BLIB_DEBUG
    static constexpr bool DefaultLogFps = true;
#else
    static constexpr bool DefaultLogFps = false;
#endif

    static constexpr const char* UpdatePeriodKey     = "blib.engine.update_period";
    static constexpr const char* MaxFpsKey           = "blib.engine.max_fps";
    static constexpr const char* VariableTimestepKey = "blib.engine.variable_timestep";
    static constexpr const char* LogFpsKey           = "blib.engine.log_fps";
    static constexpr const char* MaxEntityKey        = "blib.engine.ecs_entity_count";

    /**
     * @brief Creates a new settings object with all default settings
     */
    Settings();

    /**
     * @brief Sets the update interval to use for fixed physics updates
     *
     * @param interval Fixed timestep interval, in seconds
     * @return Settings& Reference to this object
     */
    Settings& withUpdateInterval(float interval);

    /**
     * @brief Sets the maximum framerate in fps. Pass 0 for no cap
     *
     * @param maxFps Maximum framerate in fps
     * @return Settings& Reference to this object
     */
    Settings& withMaxFramerate(float maxFps);

    /**
     * @brief Sets whether or not the engine may vary the update interval to catchup if the
     *        simulation falls behind. If a variable timestep is allowed, the update interval
     *        will be increased to match how long the updates take on average. The update
     *        interval may be adjusted down if performance improves. If a variable timestep is
     *        not allowed then updates will be skipped in order to catchup to real time.
     *
     * @param allow True to allow a variable timestep, false to skip frames to catchup
     * @return Settings& A reference to this object
     */
    Settings& withAllowVariableTimestep(bool allow);

    /**
     * @brief Sets whether or not to log FPS
     *
     * @param log True to log FPS, false to not
     * @return Settings& A reference to this object
     */
    Settings& withLogFps(bool log);

    /**
     * @brief Provides the creation settings for the renderer. If not set then no window
     *
     * @param settings The renderer creation settings
     * @return A reference to this object
     */
    Settings& withRenderer(const rc::CreationSettings& settings);

    /**
     * @brief Loads the settings from the global engine config. See Settings.cpp for keys
     *
     * @return Settings& A reference to this object
     */
    Settings& fromConfig();

    /**
     * @brief Saves the settings to the global engine config
     */
    void syncToConfig() const;

    /**
     * @brief Returns the fixed physics update interval, in seconds
     */
    float updateTimestep() const;

    /**
     * @brief Returns the fixed physics update interval, in microseconds
     */
    std::uint64_t updateTimestepMicroseconds() const;

    /**
     * @brief Returns the maximum rendering framerate, in fps
     */
    float maximumFramerate() const;

    /**
     * @brief Returns whether or not a variable timestep may be used
     */
    bool allowVariableTimestep() const;

    /**
     * @brief Returns the creation settings of the renderer
     */
    const std::optional<rc::CreationSettings>& getRendererCreationSettings() const;

    /**
     * @brief Returns whether or not a window should be created
     */
    bool createRenderer() const;

    /**
     * @brief Returns whether or not the engine should log the fps
     */
    bool logFps() const;

private:
    float updateTime;
    float maxFps;
    bool allowVariableInterval;
    bool loggingFps;
    std::optional<rc::CreationSettings> rendererSettings;
};

} // namespace engine
} // namespace bl

#endif
