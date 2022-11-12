#include <BLIB/Engine/Settings.hpp>

#include <BLIB/Engine/Configuration.hpp>
#include <BLIB/Logging.hpp>

namespace bl
{
namespace engine
{
namespace
{
const std::string UpdatePeriodKey     = "blib.engine.update_period";
const std::string MaxFpsKey           = "blib.engine.max_fps";
const std::string VariableTimestepKey = "blib.engine.variable_timestep";
const std::string LogFpsKey           = "blib.engine.log_fps";
const std::string MaxEntityKey        = "blib.engine.ecs_entity_count";

const std::string WindowWidthKey     = "blib.engine.window_width";
const std::string WindowHeightKey    = "blib.engine.window_height";
const std::string WindowBitDepthKey  = "blib.engine.window_bit_depth";
const std::string WindowStyleKey     = "blib.engine.window_style";
const std::string WindowTitleKey     = "blib.engine.window_title";
const std::string WindowLetterboxKey = "blib.engine.leterbox";
const std::string WindowIconKey      = "blib.engine.window_icon";
} // namespace

const sf::VideoMode Settings::WindowParameters::DefaultVideoMode(800, 600, 32);
const std::string Settings::WindowParameters::DefaultWindowTitle = "BLIB Engine Window";

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

    if (Configuration::getOrDefault<unsigned int>(WindowWidthKey, 0) != 0 ||
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
, letterBoxVal(DefaultLetterBoxOnResize) {}

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
}

const std::string& Settings::WindowParameters::icon() const { return iconPath; }

const std::string& Settings::WindowParameters::title() const { return sfWindowTitle; }

const sf::VideoMode& Settings::WindowParameters::videoMode() const { return windowMode; }

sf::Uint32 Settings::WindowParameters::style() const { return sfWindowStyle; }

bool Settings::WindowParameters::letterBox() const { return letterBoxVal; }

} // namespace engine
} // namespace bl
