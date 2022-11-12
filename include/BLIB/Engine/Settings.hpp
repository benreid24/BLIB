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
 *
 */
class Settings {
public:
    /**
     * @brief Collection of settings for creating (or re-creating) the game window
     *
     * @ingroup Engine
     *
     */
    class WindowParameters {
    public:
        static constexpr sf::Uint32 DefaultWindowStyle = sf::Style::Titlebar | sf::Style::Close;
        static const sf::VideoMode DefaultVideoMode;
        static const std::string DefaultWindowTitle;
        static constexpr bool DefaultLetterBoxOnResize = true;

        /**
         * @brief Construct a new WindowParameters with default settings
         *
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
         * @brief Loads the settings from the global engine config. See Settings.cpp for keys
         *
         * @return Settings& A reference to this object
         */
        WindowParameters& fromConfig();

        /**
         * @brief Saves the settings to the global engine config
         *
         */
        void syncToConfig() const;

        /**
         * @brief Returns the title to create the window with
         *
         */
        const std::string& title() const;

        /**
         * @brief Returns the video mode the engine window is created with
         *
         */
        const sf::VideoMode& videoMode() const;

        /**
         * @brief Returns the window style the window is created with
         *
         */
        sf::Uint32 style() const;

        /**
         * @brief Returns the window icon to use
         *
         */
        const std::string& icon() const;

        /**
         * @brief Returns whether or not to letterbox on Window resize
         *
         */
        bool letterBox() const;

    private:
        std::string sfWindowTitle;
        sf::VideoMode windowMode;
        sf::Uint32 sfWindowStyle;
        std::string iconPath;
        bool letterBoxVal;
    };

    static constexpr float DefaultUpdateInterval       = 1.f / 120.f;
    static constexpr float DefaultMaximumFramerate     = 0.f;
    static constexpr bool DefaultAllowVariableTimestep = true;
    static constexpr bool DefaultCreateWindow          = true;
    static constexpr bool DefaultLogFps                = false;
    static constexpr unsigned int DefaultMaxEntityCount = 2000;

    /**
     * @brief Creates a new settings object with all default settings
     *
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
     * @brief Sets the maximum number of entities to allocate for in the ECS
     *
     * @param maxEntities The number of entities to allocate memory for
     * @return Settings& A reference to this object
     */
    Settings& withMaxEntityCount(unsigned int maxEntities);

    /**
     * @brief Loads the settings from the global engine config. See Settings.cpp for keys
     *
     * @return Settings& A reference to this object
     */
    Settings& fromConfig();

    /**
     * @brief Saves the settings to the global engine config
     *
     */
    void syncToConfig() const;

    /**
     * @brief Returns the fixed physics update interval, in seconds
     *
     */
    float updateTimestep() const;

    /**
     * @brief Returns the maximum rendering framerate, in fps
     *
     */
    float maximumFramerate() const;

    /**
     * @brief Returns whether or not a variable timestep may be used
     *
     */
    bool allowVariableTimestep() const;

    /**
     * @brief Returns whether or not a window should be created
     *
     */
    bool createWindow() const;

    /**
     * @brief Returns the parameters to create the window with. Undefined behavior if createWindow()
     *        returns false
     *
     */
    const WindowParameters& windowParameters() const;

    /**
     * @brief Returns whether or not the engine should log the fps
     *
     */
    bool logFps() const;

    /**
     * @brief Returns the maximum number of entities in the ECS
     * 
     */
    unsigned int maximumEntityCount() const;

private:
    float updateTime;
    float maxFps;
    bool allowVariableInterval;
    std::optional<WindowParameters> windowParams;
    bool loggingFps;
    unsigned int maxEntities;
};

} // namespace engine
} // namespace bl

#endif
