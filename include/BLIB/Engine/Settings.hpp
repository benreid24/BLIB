#ifndef BLIB_ENGINE_SETTINGS_HPP
#define BLIB_ENGINE_SETTINGS_HPP

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
    /**
     * @brief Collection of settings for creating (or re-creating) the game window
     *
     * @ingroup Engine
     */
    class WindowParameters {
    public:
        static constexpr sf::Uint32 DefaultWindowStyle = sf::Style::Titlebar | sf::Style::Close;
        static const sf::VideoMode DefaultVideoMode;
        static const std::string DefaultWindowTitle;
        static constexpr bool DefaultLetterBoxOnResize = true;
        static constexpr bool DefaultSyncOverlaySize   = false;
        static const sf::Vector2f DefaultViewSize;
        static constexpr bool DefaultVSyncEnabled = true;

        /**
         * @brief Construct a new WindowParameters with default settings
         */
        WindowParameters();

        /**
         * @brief Sets the title to create the window with
         *
         * @param title The title to create the window with
         * @return WindowParameters& A reference to this object
         */
        WindowParameters& withTitle(const std::string& title);

        /**
         * @brief Sets the video mode to create the engine window with
         *
         * @param mode The window resolution and color depth
         * @return WindowParameters& A reference to this object
         */
        WindowParameters& withVideoMode(const sf::VideoMode& mode);

        /**
         * @brief Sets the style to create the engine window with
         *
         * @param style The style to create the window with
         * @return WindowParameters& A reference to this object
         */
        WindowParameters& withStyle(sf::Uint32 style);

        /**
         * @brief Sets the path to an image to use as the window icon
         *
         * @param iconPath Path to the image to use
         * @return Settings& A reference to this object
         */
        WindowParameters& withIcon(const std::string& iconPath);

        /**
         * @brief Sets whether or not to letterbox when the window is resized
         *
         * @param letterBox True to letterbox, false to keep the full view
         * @return WindowParameters& A reference to this object
         */
        WindowParameters& withLetterBoxOnResize(bool letterBox);

        /**
         * @brief Size to set the view to when the window is created. If this is unspecified then
         *        the video mode size is used. The size specified here is used for letterboxing
         *
         * @param viewSize The initial size of the window view
         * @return WindowParameters& A reference to this object
         */
        WindowParameters& withInitialViewSize(const sf::Vector2f& viewSize);

        /**
         * @brief Sets whether to sync Overlay coordinate space with the window size. Only applies
         *        if letter boxing is disabled. Default is disabled
         *
         * @param sync True to sync Overlay coordinate space to window size, false to keep original
         * @return A reference to this object
         */
        WindowParameters& withSyncOverlaySizeToWindow(bool sync);

        /**
         * @brief Sets whether or not to enable vsync
         *
         * @param enabled True to enable, false to disable
         * @return WindowParameters& A reference to this object
         */
        WindowParameters& withVSyncEnabled(bool enabled);

        /**
         * @brief Loads the settings from the global engine config. See Settings.cpp for keys
         *
         * @return Settings& A reference to this object
         */
        WindowParameters& fromConfig();

        /**
         * @brief Saves the settings to the global engine config
         */
        void syncToConfig() const;

        /**
         * @brief Returns the title to create the window with
         */
        const std::string& title() const;

        /**
         * @brief Returns the video mode the engine window is created with
         */
        const sf::VideoMode& videoMode() const;

        /**
         * @brief Returns the window style the window is created with
         */
        sf::Uint32 style() const;

        /**
         * @brief Returns the window icon to use
         */
        const std::string& icon() const;

        /**
         * @brief Returns whether or not to letterbox on Window resize
         */
        bool letterBox() const;

        /**
         * @brief Returns the view size the window is created with
         */
        const sf::Vector2f& initialViewSize() const;

        /**
         * @brief Returns whether Overlay coordinate space is synced to window size or not
         */
        bool syncOverlaySize() const;

        /**
         * @brief Returns whether or not to enable vsync on the window
         */
        bool vsyncEnabled() const;

        static constexpr const char* WindowWidthKey      = "blib.engine.window_width";
        static constexpr const char* WindowHeightKey     = "blib.engine.window_height";
        static constexpr const char* WindowBitDepthKey   = "blib.engine.window_bit_depth";
        static constexpr const char* WindowStyleKey      = "blib.engine.window_style";
        static constexpr const char* WindowTitleKey      = "blib.engine.window_title";
        static constexpr const char* WindowLetterboxKey  = "blib.engine.leterbox";
        static constexpr const char* WindowIconKey       = "blib.engine.window_icon";
        static constexpr const char* WindowViewWidthKey  = "blib.engine.view_width";
        static constexpr const char* WindowViewHeightKey = "blib.engine.view_height";
        static constexpr const char* VSyncKey            = "blib.engine.window_vsync";
        static constexpr const char* SyncOverlaySizeKey  = "blib.engine.sync_overlay_size";

    private:
        std::string sfWindowTitle;
        sf::VideoMode windowMode;
        sf::Uint32 sfWindowStyle;
        std::string iconPath;
        bool letterBoxVal;
        sf::Vector2f viewSize;
        bool syncOverlay;
        bool vsync;
    };

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
     * @brief Sets the parameters to create the game window with. Does not create a window if not
     *        called at least once.
     *
     * @param parameters The parameters to create the game window with
     * @return Settings& A reference to this object
     */
    Settings& withWindowParameters(const WindowParameters& parameters);

    /**
     * @brief Sets whether or not to log FPS
     *
     * @param log True to log FPS, false to not
     * @return Settings& A reference to this object
     */
    Settings& withLogFps(bool log);

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
     * @brief Returns the maximum rendering framerate, in fps
     */
    float maximumFramerate() const;

    /**
     * @brief Returns whether or not a variable timestep may be used
     */
    bool allowVariableTimestep() const;

    /**
     * @brief Returns whether or not a window should be created
     */
    bool createWindow() const;

    /**
     * @brief Returns the parameters to create the window with. Undefined behavior if createWindow()
     *        returns false
     */
    const WindowParameters& windowParameters() const;

    /**
     * @brief Returns whether or not the engine should log the fps
     */
    bool logFps() const;

private:
    float updateTime;
    float maxFps;
    bool allowVariableInterval;
    std::optional<WindowParameters> windowParams;
    bool loggingFps;
};

} // namespace engine
} // namespace bl

#endif
