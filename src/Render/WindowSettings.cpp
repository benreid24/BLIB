#include <BLIB/Render/WindowSettings.hpp>

#include <BLIB/Engine/Configuration.hpp>

namespace bl
{
namespace rc
{
const sf::VideoMode WindowSettings::DefaultVideoMode({1920, 1080}, 32);
const std::string WindowSettings::DefaultWindowTitle = "BLIB Engine Window";
const sf::Vector2f WindowSettings::DefaultViewSize(0.f, 0.f);

WindowSettings::WindowSettings()
: sfWindowTitle(DefaultWindowTitle)
, windowMode(DefaultVideoMode)
, sfWindowStyle(DefaultWindowStyle)
, iconPath()
, letterBoxVal(DefaultLetterBoxOnResize)
, viewSize(DefaultViewSize)
, syncOverlay(DefaultSyncOverlaySize)
, vsync(DefaultVSyncEnabled)
, changesRequireWindowRecreate(false) {}

WindowSettings& WindowSettings::withTitle(const std::string& title) {
    sfWindowTitle = title;
    return *this;
}

WindowSettings& WindowSettings::withVideoMode(const sf::VideoMode& mode) {
    if (windowMode != mode) { changesRequireWindowRecreate = true; }
    windowMode = mode;
    return *this;
}

WindowSettings& WindowSettings::withStyle(std::uint32_t style) {
    if (sfWindowStyle != style) { changesRequireWindowRecreate = true; }
    sfWindowStyle = style;
    return *this;
}

WindowSettings& WindowSettings::withState(sf::State state) {
    const std::uint32_t s = static_cast<std::uint32_t>(state);
    if (windowState != s) { changesRequireWindowRecreate = true; }
    windowState = s;
    return *this;
}

WindowSettings& WindowSettings::withLetterBoxOnResize(bool lb) {
    letterBoxVal = lb;
    return *this;
}

WindowSettings& WindowSettings::withSyncOverlaySizeToWindow(bool s) {
    syncOverlay = s;
    return *this;
}

WindowSettings& WindowSettings::withIcon(const std::string& path) {
    iconPath = path;
    return *this;
}

WindowSettings& WindowSettings::withInitialViewSize(const sf::Vector2f& s) {
    viewSize = s;
    return *this;
}

WindowSettings& WindowSettings::withVSyncEnabled(bool enabled) {
    vsync = enabled;
    return *this;
}

WindowSettings& WindowSettings::fromConfig(const std::string& prefix) {
    sfWindowTitle =
        engine::Configuration::getOrDefault<std::string>(prefix + TitleKey, sfWindowTitle);
    windowMode.size.x =
        engine::Configuration::getOrDefault<unsigned int>(prefix + WidthKey, windowMode.size.x);
    windowMode.size.y =
        engine::Configuration::getOrDefault<unsigned int>(prefix + HeightKey, windowMode.size.y);
    windowMode.bitsPerPixel = engine::Configuration::getOrDefault<unsigned int>(
        prefix + BitDepthKey, windowMode.bitsPerPixel);
    sfWindowStyle =
        engine::Configuration::getOrDefault<unsigned int>(prefix + StyleKey, sfWindowStyle);
    windowState = engine::Configuration::getOrDefault<unsigned int>(prefix + StateKey, windowState);
    letterBoxVal = engine::Configuration::getOrDefault<bool>(prefix + LetterboxKey, letterBoxVal);
    iconPath     = engine::Configuration::getOrDefault<std::string>(prefix + IconKey, iconPath);
    viewSize.x   = engine::Configuration::getOrDefault<float>(prefix + ViewWidthKey, viewSize.x);
    viewSize.y   = engine::Configuration::getOrDefault<float>(prefix + ViewHeightKey, viewSize.y);
    vsync        = engine::Configuration::getOrDefault<bool>(prefix + VSyncKey, vsync);
    syncOverlay =
        engine::Configuration::getOrDefault<bool>(prefix + SyncOverlaySizeKey, syncOverlay);

    return *this;
}

void WindowSettings::syncToConfig(const std::string& prefix) const {
    engine::Configuration::set<std::string>(prefix + TitleKey, sfWindowTitle);
    engine::Configuration::set<unsigned int>(prefix + WidthKey, windowMode.size.x);
    engine::Configuration::set<unsigned int>(prefix + HeightKey, windowMode.size.y);
    engine::Configuration::set<unsigned int>(prefix + BitDepthKey, windowMode.bitsPerPixel);
    engine::Configuration::set<unsigned int>(prefix + StyleKey, sfWindowStyle);
    engine::Configuration::set<unsigned int>(prefix + StateKey, windowState);
    engine::Configuration::set<bool>(prefix + LetterboxKey, letterBoxVal);
    engine::Configuration::set<std::string>(prefix + IconKey, iconPath);
    engine::Configuration::set<float>(prefix + ViewWidthKey, viewSize.x);
    engine::Configuration::set<float>(prefix + ViewHeightKey, viewSize.y);
    engine::Configuration::set<bool>(prefix + VSyncKey, vsync);
}

const std::string& WindowSettings::icon() const { return iconPath; }

const std::string& WindowSettings::title() const { return sfWindowTitle; }

const sf::VideoMode& WindowSettings::videoMode() const { return windowMode; }

std::uint32_t WindowSettings::style() const { return sfWindowStyle; }

std::uint32_t WindowSettings::state() const { return windowState; }

bool WindowSettings::letterBox() const { return letterBoxVal; }

const sf::Vector2f& WindowSettings::initialViewSize() const { return viewSize; }

bool WindowSettings::vsyncEnabled() const { return vsync; }

bool WindowSettings::syncOverlaySize() const { return syncOverlay; }

bool WindowSettings::changesRequireNewWindow() {
    const bool v                 = changesRequireWindowRecreate;
    changesRequireWindowRecreate = false;
    return v;
}

} // namespace rc
} // namespace bl
