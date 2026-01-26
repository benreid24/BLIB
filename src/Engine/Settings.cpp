#include <BLIB/Engine/Settings.hpp>

#include <BLIB/Engine/Configuration.hpp>
#include <BLIB/Logging.hpp>

namespace bl
{
namespace engine
{
namespace
{
constexpr const char* RendererCreateKey = "blib.engine.create_renderer";
constexpr const char* RendererPrefix    = "blib.engine.renderer.0";
} // namespace

Settings::Settings()
: updateTime(DefaultUpdateInterval)
, maxFps(DefaultMaximumFramerate)
, allowVariableInterval(DefaultAllowVariableTimestep)
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

Settings& Settings::withLogFps(bool log) {
    loggingFps = log;
    return *this;
}

Settings& Settings::withRenderer(const rc::CreationSettings& settings) {
    rendererSettings = settings;
    return *this;
}

Settings& Settings::fromConfig() {
    updateTime = Configuration::getOrDefault<float>(UpdatePeriodKey, updateTime);
    maxFps     = Configuration::getOrDefault<float>(MaxFpsKey, maxFps);
    allowVariableInterval =
        Configuration::getOrDefault<bool>(VariableTimestepKey, allowVariableInterval);
    loggingFps = Configuration::getOrDefault<bool>(LogFpsKey, loggingFps);

    if (Configuration::getOrDefault<bool>(RendererCreateKey, false) ||
        rendererSettings.has_value()) {
        if (!rendererSettings.has_value()) { rendererSettings.emplace(); }
        rendererSettings.value().fromConfig(RendererPrefix);
    }

    return *this;
}

void Settings::syncToConfig() const {
    Configuration::set<float>(UpdatePeriodKey, updateTime);
    Configuration::set<float>(MaxFpsKey, maxFps);
    Configuration::set<bool>(VariableTimestepKey, allowVariableInterval);
    Configuration::set<bool>(LogFpsKey, loggingFps);
    if (rendererSettings.has_value()) { rendererSettings.value().syncToConfig(RendererPrefix); }
}

float Settings::updateTimestep() const { return updateTime; }

std::uint64_t Settings::updateTimestepMicroseconds() const {
    return static_cast<std::uint64_t>(updateTime * 1'000'000.f);
}

float Settings::maximumFramerate() const { return maxFps; }

bool Settings::allowVariableTimestep() const { return allowVariableInterval; }

bool Settings::createRenderer() const { return rendererSettings.has_value(); }

bool Settings::logFps() const { return loggingFps; }

const std::optional<rc::CreationSettings>& Settings::getRendererCreationSettings() const {
    return rendererSettings;
}

} // namespace engine
} // namespace bl
