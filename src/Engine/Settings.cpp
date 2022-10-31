#include <BLIB/Engine/Settings.hpp>

#include <BLIB/Engine/Configuration.hpp>
#include <BLIB/Logging.hpp>

namespace bl
{
namespace engine
{
const sf::VideoMode Settings::WindowParameters::DefaultVideoMode(800, 600, 32);
const std::string Settings::WindowParameters::DefaultWindowTitle = "BLIB Engine Window";

Settings::Settings()
: updateTime(DefaultUpdateInterval)
, maxFps(DefaultMaximumFramerate)
, allowVariableInterval(DefaultAllowVariableTimestep)
, windowParams()
, loggingFps(DefaultLogFps) {}

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

Settings& Settings::withWindowParameters(Settings::WindowParameters&& params) {
    windowParams = std::forward<Settings::WindowParameters>(params);
    return *this;
}

Settings& Settings::withLogFps(bool log) {
    loggingFps = log;
    return *this;
}

float Settings::updateTimestep() const { return updateTime; }

float Settings::maximumFramerate() const { return maxFps; }

bool Settings::allowVariableTimestep() const { return allowVariableInterval; }

bool Settings::createWindow() const { return windowParams.has_value(); }

const Settings::WindowParameters& Settings::windowParameters() const {
    return windowParams.value();
}

bool Settings::logFps() const { return loggingFps; }

Settings::WindowParameters::WindowParameters()
: sfWindowTitle(DefaultWindowTitle)
, windowMode(DefaultVideoMode)
, sfWindowStyle(DefaultWindowStyle)
, iconImg()
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
    if (!iconImg.loadFromFile(path)) { BL_LOG_ERROR << "Failed to load window icon: " << path; }
    return *this;
}

const sf::Image& Settings::WindowParameters::icon() const { return iconImg; }

const std::string& Settings::WindowParameters::title() const { return sfWindowTitle; }

const sf::VideoMode& Settings::WindowParameters::videoMode() const { return windowMode; }

sf::Uint32 Settings::WindowParameters::style() const { return sfWindowStyle; }

bool Settings::WindowParameters::letterBox() const { return letterBoxVal; }

} // namespace engine
} // namespace bl
