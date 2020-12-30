#include <BLIB/Engine/Settings.hpp>

namespace bl
{
namespace engine
{
Settings::Settings()
: updateTime(DefaultUpdateInterval)
, maxFps(DefaultMaximumFramerate)
, allowVariableInterval(DefaultAllowVariableTimestep) {}

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

float Settings::updateTimestep() const { return updateTime; }

float Settings::maximumFramerate() const { return maxFps; }

bool Settings::allowVariableTimestep() const { return allowVariableInterval; }

} // namespace engine
} // namespace bl
