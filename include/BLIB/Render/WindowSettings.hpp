#ifndef BLIB_RENDER_WINDOWSETTINGS_HPP
#define BLIB_RENDER_WINDOWSETTINGS_HPP

#include <SFML/System/Vector2.hpp>
#include <SFML/Window/VideoMode.hpp>
#include <SFML/Window/WindowEnums.hpp>
#include <cstdint>
#include <string>

namespace bl
{
namespace rc
{
/**
 * @brief Collection of settings for creating (or re-creating) the game window
 *
 * @ingroup Renderer
 */
class WindowSettings {
public:
    static constexpr std::uint32_t DefaultWindowStyle = sf::Style::Titlebar | sf::Style::Close;
    static const sf::VideoMode DefaultVideoMode;
    static const std::string DefaultWindowTitle;
    static constexpr bool DefaultLetterBoxOnResize = true;
    static constexpr bool DefaultSyncOverlaySize   = false;
    static const sf::Vector2f DefaultViewSize;
    static constexpr bool DefaultVSyncEnabled = true;

    /**
     * @brief Construct a new WindowSettings with default settings
     */
    WindowSettings();

    /**
     * @brief Sets the title to create the window with
     *
     * @param title The title to create the window with
     * @return WindowSettings& A reference to this object
     */
    WindowSettings& withTitle(const std::string& title);

    /**
     * @brief Sets the video mode to create the engine window with
     *
     * @param mode The window resolution and color depth
     * @return WindowSettings& A reference to this object
     */
    WindowSettings& withVideoMode(const sf::VideoMode& mode);

    /**
     * @brief Sets the style to create the engine window with
     *
     * @param style The style to create the window with
     * @return WindowSettings& A reference to this object
     */
    WindowSettings& withStyle(std::uint32_t style);

    /**
     * @brief Sets the state of the window
     *
     * @param state Fullscreen or windowed
     * @return A reference to this object
     */
    WindowSettings& withState(sf::State state);

    /**
     * @brief Sets the path to an image to use as the window icon
     *
     * @param iconPath Path to the image to use
     * @return Settings& A reference to this object
     */
    WindowSettings& withIcon(const std::string& iconPath);

    /**
     * @brief Sets whether or not to letterbox when the window is resized
     *
     * @param letterBox True to letterbox, false to keep the full view
     * @return WindowSettings& A reference to this object
     */
    WindowSettings& withLetterBoxOnResize(bool letterBox);

    /**
     * @brief Size to set the view to when the window is created. If this is unspecified then
     *        the video mode size is used. The size specified here is used for letterboxing
     *
     * @param viewSize The initial size of the window view
     * @return WindowSettings& A reference to this object
     */
    WindowSettings& withInitialViewSize(const sf::Vector2f& viewSize);

    /**
     * @brief Sets whether to sync Overlay coordinate space with the window size. Only applies
     *        if letter boxing is disabled. Default is disabled
     *
     * @param sync True to sync Overlay coordinate space to window size, false to keep original
     * @return A reference to this object
     */
    WindowSettings& withSyncOverlaySizeToWindow(bool sync);

    /**
     * @brief Sets whether or not to enable vsync
     *
     * @param enabled True to enable, false to disable
     * @return WindowSettings& A reference to this object
     */
    WindowSettings& withVSyncEnabled(bool enabled);

    /**
     * @brief Loads the settings from the global engine config. See Settings.cpp for keys
     *
     * @param keyPrefix The prefix to use when loading from the config
     * @return Settings& A reference to this object
     */
    WindowSettings& fromConfig(const std::string& keyPrefix);

    /**
     * @brief Saves the settings to the global engine config
     *
     * @param keyPrefix The prefix to use when saving to the config
     */
    void syncToConfig(const std::string& keyPrefix) const;

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
    std::uint32_t style() const;

    /**
     * @brief Returns the state the window is created with
     */
    std::uint32_t state() const;

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

    /**
     * @brief Returns whether any changes to the window settings require the window to be recreated
     */
    bool changesRequireNewWindow();

    static constexpr const char* WidthKey           = ".window.width";
    static constexpr const char* HeightKey          = ".window.height";
    static constexpr const char* BitDepthKey        = ".window.bit_depth";
    static constexpr const char* StyleKey           = ".window.style";
    static constexpr const char* StateKey           = ".window.state";
    static constexpr const char* TitleKey           = ".window.title";
    static constexpr const char* LetterboxKey       = ".window.leterbox";
    static constexpr const char* IconKey            = ".window.icon";
    static constexpr const char* ViewWidthKey       = ".window.view_width";
    static constexpr const char* ViewHeightKey      = ".window.view_height";
    static constexpr const char* VSyncKey           = ".window.vsync";
    static constexpr const char* SyncOverlaySizeKey = ".window.sync_overlay_size";

private:
    std::string sfWindowTitle;
    sf::VideoMode windowMode;
    std::uint32_t sfWindowStyle;
    std::uint32_t windowState;
    std::string iconPath;
    bool letterBoxVal;
    sf::Vector2f viewSize;
    bool syncOverlay;
    bool vsync;

    bool changesRequireWindowRecreate;
};

} // namespace rc
} // namespace bl

#endif
