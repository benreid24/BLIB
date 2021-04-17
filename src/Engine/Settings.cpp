#include <BLIB/Engine/Settings.hpp>
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

float Settings::updateTimestep() const { return updateTime; }

float Settings::maximumFramerate() const { return maxFps; }

bool Settings::allowVariableTimestep() const { return allowVariableInterval; }

const std::string& Settings::windowTitle() const { return sfWindowTitle; }

const sf::VideoMode& Settings::videoMode() const { return windowMode; }

sf::Uint32 Settings::windowStyle() const { return sfWindowStyle; }

bool Settings::createWindow() const { return createSfWindow; }

const sf::Image& Settings::windowIcon() const { return icon; }

} // namespace engine
} // namespace bl
