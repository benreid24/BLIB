#include <BLIB/Engine/Settings.hpp>

#include <BLIB/Engine/Configuration.hpp>
#include <BLIB/Logging.hpp>

namespace bl
{
namespace engine
{
const sf::VideoMode Settings::DefaultVideoMode(800, 600, 32);
const std::string Settings::DefaultWindowTitle = "BLIB Engine Window";

Settings::Settings()
: updateTime(DefaultUpdateInterval)
, maxFps(DefaultMaximumFramerate)
, allowVariableInterval(DefaultAllowVariableTimestep)
, sfWindowTitle(DefaultWindowTitle)
, windowMode(DefaultVideoMode)
, sfWindowStyle(DefaultWindowStyle)
, createSfWindow(DefaultCreateWindow)
, loggingFps(DefaultLogFps)
, icon() {}

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

Settings& Settings::withWindowTitle(const std::string& title) {
    sfWindowTitle = title;
    return *this;
}

Settings& Settings::withVideoMode(const sf::VideoMode& mode) {
    windowMode = mode;
    return *this;
}

Settings& Settings::withWindowStyle(sf::Uint32 style) {
    sfWindowStyle = style;
    return *this;
}

Settings& Settings::withCreateWindow(bool c) {
    createSfWindow = c;
    return *this;
}

Settings& Settings::withWindowIcon(const std::string& path) {
    if (!icon.loadFromFile(path)) { BL_LOG_ERROR << "Failed to load window icon: " << path; }
    return *this;
}

Settings& Settings::withLogFps(bool log) {
    loggingFps = log;
    return *this;
}

Settings& Settings::fromConfig() {
    updateTime = Configuration::getOrDefault<float>("blib.engine.update_period", updateTime);
    maxFps     = Configuration::getOrDefault<float>("blib.engine.max_fps", maxFps);
    allowVariableInterval =
        Configuration::getOrDefault<bool>("blib.engine.variable_timestep", allowVariableInterval);
    sfWindowTitle =
        Configuration::getOrDefault<std::string>("blib.engine.window_title", sfWindowTitle);
    windowMode.width =
        Configuration::getOrDefault<unsigned int>("blib.engine.window_width", windowMode.width);
    windowMode.height =
        Configuration::getOrDefault<unsigned int>("blib.engine.window_height", windowMode.height);
    loggingFps = Configuration::getOrDefault<bool>("blib.engine.log_fps", loggingFps);

    const std::string icf = Configuration::getOrDefault<std::string>("blib.engine.icon", "");
    if (!icf.empty()) {
        if (!icon.loadFromFile(icf)) BL_LOG_ERROR << "Failed to load window icon: " << icf;
    }

    return *this;
}

float Settings::updateTimestep() const { return updateTime; }

float Settings::maximumFramerate() const { return maxFps; }

bool Settings::allowVariableTimestep() const { return allowVariableInterval; }

const std::string& Settings::windowTitle() const { return sfWindowTitle; }

const sf::VideoMode& Settings::videoMode() const { return windowMode; }

sf::Uint32 Settings::windowStyle() const { return sfWindowStyle; }

bool Settings::createWindow() const { return createSfWindow; }

const sf::Image& Settings::windowIcon() const { return icon; }

bool Settings::logFps() const { return loggingFps; }

} // namespace engine
} // namespace bl
