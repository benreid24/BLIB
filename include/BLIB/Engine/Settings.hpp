#ifndef BLIB_ENGINE_SETTINGS_HPP
#define BLIB_ENGINE_SETTINGS_HPP

#include <SFML/Window.hpp>
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
    static constexpr float DefaultUpdateInterval       = 1.f / 120.f;
    static constexpr float DefaultMaximumFramerate     = 0.f;
    static constexpr bool DefaultAllowVariableTimestep = true;
    static const std::string DefaultWindowTitle;
    static const sf::VideoMode DefaultVideoMode;
    static constexpr sf::Uint32 DefaultWindowStyle = sf::Style::Titlebar | sf::Style::Close;
    static constexpr bool DefaultCreateWindow      = false;

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
     * @brief Sets the title to create the window with
     *
     * @param title The title to create the window with
     * @return Settings& A reference to this object
     */
    Settings& withWindowTitle(const std::string& title);

    /**
     * @brief Sets the video mode to create the engine window with
     *
     * @param mode The window resolution and color depth
     * @return Settings& A reference to this object
     */
    Settings& withVideoMode(const sf::VideoMode& mode);

    /**
     * @brief Sets the style to create the engine window with
     *
     * @param style The style to create the window with
     * @return Settgins& A reference to this object
     */
    Settings& withWindowStyle(sf::Uint32 style);

    /**
     * @brief Sets whether or not the engine creates a sf::RenderWindow. Set to false for test
     *        environments or other cases where a window is not desired
     *
     * @param create True to create a window, false to run windowless
     * @return Settings& A reference to this object
     */
    Settings& withCreateWindow(bool create);

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
     * @brief Returns the title to create the window with
     *
     */
    const std::string& windowTitle() const;

    /**
     * @brief Returns the video mode the engine window is created with
     *
     */
    const sf::VideoMode& videoMode() const;

    /**
     * @brief Returns the window style the window is created with
     *
     */
    sf::Uint32 windowStyle() const;

    /**
     * @brief Returns whether or not a window should be created
     *
     */
    bool createWindow() const;

private:
    float updateTime;
    float maxFps;
    bool allowVariableInterval;
    std::string sfWindowTitle;
    sf::VideoMode windowMode;
    sf::Uint32 sfWindowStyle;
    bool createSfWindow;
};

} // namespace engine
} // namespace bl

#endif
