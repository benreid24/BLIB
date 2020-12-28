#include <BLIB/Engine/EngineSettings.hpp>

namespace bl
{
EngineSettings::EngineSettings()
: updateTime(DefaultUpdateInterval)
, maxFps(DefaultMaximumFramerate)
, allowVariableInterval(DefaultAllowVariableTimestep) {}

EngineSettings& EngineSettings::withMaxFramerate(float fps) {
    maxFps = fps;
    return *this;
}

EngineSettings& EngineSettings::withUpdateInterval(float interval) {
    updateTime = interval;
    return *this;
}

EngineSettings& EngineSettings::withAllowVariableTimestep(bool allow) {
    allowVariableInterval = allow;
    return *this;
}

float EngineSettings::updateTimestep() const { return updateTime; }

float EngineSettings::maximumFramerate() const { return maxFps; }

bool EngineSettings::allowVariableTimestep() const { return allowVariableInterval; }

} // namespace bl
