#include <BLIB/Engine/Settings.hpp>

#include <BLIB/Engine/Configuration.hpp>
#include <BLIB/Logging.hpp>

namespace bl
{
namespace engine
{
const sf::VideoMode Settings::WindowParameters::DefaultVideoMode(800, 600, 32);
const std::string Settings::WindowParameters::DefaultWindowTitle = "BLIB Engine Window";
const sf::Vector2f Settings::WindowParameters::DefaultViewSize(0.f, 0.f);

Settings::Settings()
: updateTime(DefaultUpdateInterval)
, maxFps(DefaultMaximumFramerate)
, allowVariableInterval(DefaultAllowVariableTimestep)
, windowParams()
, loggingFps(DefaultLogFps)
, maxEntities(DefaultMaxEntityCount) {}

Settings& Settings::withMaxFramerate(float fps) {
    maxFps = fps;
    return *this;
}

Settings& Settings::withUpdateInterval(float interval) {
    updateTime = interval;
    return *this;
}

Settings& Settings::withAllowVariableTimestep(bool allow) {
    allowVariableInterval = allow;
    return *this;
}

Settings& Settings::withWindowParameters(const Settings::WindowParameters& params) {
    windowParams = params;
    return *this;
}

Settings& Settings::withLogFps(bool log) {
    loggingFps = log;
    return *this;
}

Settings& Settings::withMaxEntityCount(unsigned int m) {
    maxEntities = m;
    return *this;
}

Settings& Settings::fromConfig() {
    updateTime = Configuration::getOrDefault<float>(UpdatePeriodKey, updateTime);
    maxFps     = Configuration::getOrDefault<float>(MaxFpsKey, maxFps);
    allowVariableInterval =
        Configuration::getOrDefault<bool>(VariableTimestepKey, allowVariableInterval);
    loggingFps  = Configuration::getOrDefault<bool>(LogFpsKey, loggingFps);
    maxEntities = Configuration::getOrDefault<unsigned int>(MaxEntityKey, maxEntities);

    if (Configuration::getOrDefault<unsigned int>(WindowParameters::WindowWidthKey, 0) != 0 ||
        windowParams.has_value()) {
        if (!windowParams.has_value()) { windowParams.emplace(); }
        windowParams.value().fromConfig();
    }

    return *this;
}

void Settings::syncToConfig() const {
    Configuration::set<float>(UpdatePeriodKey, updateTime);
    Configuration::set<float>(MaxFpsKey, maxFps);
    Configuration::set<bool>(VariableTimestepKey, allowVariableInterval);
    Configuration::set<bool>(LogFpsKey, loggingFps);
    Configuration::set<unsigned int>(MaxEntityKey, maxEntities);
    if (windowParams.has_value()) { windowParams.value().syncToConfig(); }
}

float Settings::updateTimestep() const { return updateTime; }

float Settings::maximumFramerate() const { return maxFps; }

bool Settings::allowVariableTimestep() const { return allowVariableInterval; }

bool Settings::createWindow() const { return windowParams.has_value(); }

const Settings::WindowParameters& Settings::windowParameters() const {
    return windowParams.value();
}

bool Settings::logFps() const { return loggingFps; }

unsigned int Settings::maximumEntityCount() const { return maxEntities; }

Settings::WindowParameters::WindowParameters()
: sfWindowTitle(DefaultWindowTitle)
, windowMode(DefaultVideoMode)
, sfWindowStyle(DefaultWindowStyle)
, iconPath()
, letterBoxVal(DefaultLetterBoxOnResize)
, viewSize(DefaultViewSize)
, vsync(DefaultVSyncEnabled) {}

Settings::WindowParameters& Settings::WindowParameters::withTitle(const std::string& title) {
    sfWindowTitle = title;
    return *this;
}

Settings::WindowParameters& Settings::WindowParameters::withVideoMode(const sf::VideoMode& mode) {
    windowMode = mode;
    return *this;
}

Settings::WindowParameters& Settings::WindowParameters::withStyle(sf::Uint32 style) {
    sfWindowStyle = style;
    return *this;
}

Settings::WindowParameters& Settings::WindowParameters::withLetterBoxOnResize(bool lb) {
    letterBoxVal = lb;
    return *this;
}

Settings::WindowParameters& Settings::WindowParameters::withIcon(const std::string& path) {
    iconPath = path;
    return *this;
}

Settings::WindowParameters& Settings::WindowParameters::withInitialViewSize(const sf::Vector2f& s) {
    viewSize = s;
    return *this;
}

Settings::WindowParameters& Settings::WindowParameters::withVSyncEnabled(bool enabled) {
    vsync = enabled;
    return *this;
}

Settings::WindowParameters& Settings::WindowParameters::fromConfig() {
    sfWindowTitle    = Configuration::getOrDefault<std::string>(WindowTitleKey, sfWindowTitle);
    windowMode.width = Configuration::getOrDefault<unsigned int>(WindowWidthKey, windowMode.width);
    windowMode.height =
        Configuration::getOrDefault<unsigned int>(WindowHeightKey, windowMode.height);
    windowMode.bitsPerPixel =
        Configuration::getOrDefault<unsigned int>(WindowBitDepthKey, windowMode.bitsPerPixel);
    sfWindowStyle = Configuration::getOrDefault<unsigned int>(WindowStyleKey, sfWindowStyle);
    letterBoxVal  = Configuration::getOrDefault<bool>(WindowLetterboxKey, letterBoxVal);
    iconPath      = Configuration::getOrDefault<std::string>(WindowIconKey, iconPath);
    viewSize.x    = Configuration::getOrDefault<float>(WindowViewWidthKey, viewSize.x);
    viewSize.y    = Configuration::getOrDefault<float>(WindowViewHeightKey, viewSize.y);
    vsync         = Configuration::getOrDefault<bool>(VSyncKey, vsync);

    return *this;
}

void Settings::WindowParameters::syncToConfig() const {
    Configuration::set<std::string>(WindowTitleKey, sfWindowTitle);
    Configuration::set<unsigned int>(WindowWidthKey, windowMode.width);
    Configuration::set<unsigned int>(WindowHeightKey, windowMode.height);
    Configuration::set<unsigned int>(WindowBitDepthKey, windowMode.bitsPerPixel);
    Configuration::set<unsigned int>(WindowStyleKey, sfWindowStyle);
    Configuration::set<bool>(WindowLetterboxKey, letterBoxVal);
    Configuration::set<std::string>(WindowIconKey, iconPath);
    Configuration::set<float>(WindowViewWidthKey, viewSize.x);
    Configuration::set<float>(WindowViewHeightKey, viewSize.y);
    Configuration::set<bool>(VSyncKey, vsync);
}

const std::string& Settings::WindowParameters::icon() const { return iconPath; }

const std::string& Settings::WindowParameters::title() const { return sfWindowTitle; }

const sf::VideoMode& Settings::WindowParameters::videoMode() const { return windowMode; }

sf::Uint32 Settings::WindowParameters::style() const { return sfWindowStyle; }

bool Settings::WindowParameters::letterBox() const { return letterBoxVal; }

const sf::Vector2f& Settings::WindowParameters::initialViewSize() const { return viewSize; }

bool Settings::WindowParameters::vsyncEnabled() const { return vsync; }

} // namespace engine
} // namespace bl
