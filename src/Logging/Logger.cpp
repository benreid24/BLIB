#include <BLIB/Logging.hpp>

#include <BLIB/Engine/Engine.hpp>

namespace bl
{
namespace logging
{
Logger Logger::critical() { return Logger(Config::Critical); }

Logger Logger::error() { return Logger(Config::Error); }

Logger Logger::warn() { return Logger(Config::Warn); }

Logger Logger::info() { return Logger(Config::Info); }

Logger Logger::debug() { return Logger(Config::Debug); }

Logger Logger::trace() { return Logger(Config::Trace); }

Logger::Logger(int level)
: level(level) {
    ss << Config::get().genPrefix(level);
}

Logger::~Logger() {
    if (ss.str().back() != '\n') { ss << '\n'; }
    engine::Engine* engine = engine::Engine::getInstance();
    if (engine) {
        engine->fastTaskThreadpool().queueTask(
            std::bind(&Config::doWrite, &Config::get(), ss.str(), level));
    }
    else { Config::get().doWrite(ss.str(), level); }
}

} // namespace logging
} // namespace bl
