#include <BLIB/Logging.hpp>

namespace bl
{
namespace logging
{
Logger Logger::critical() { return Logger(Config::get(), Config::Critical); }

Logger Logger::error() { return Logger(Config::get(), Config::Error); }

Logger Logger::warn() { return Logger(Config::get(), Config::Warn); }

Logger Logger::info() { return Logger(Config::get(), Config::Info); }

Logger Logger::debug() { return Logger(Config::get(), Config::Debug); }

Logger Logger::trace() { return Logger(Config::get(), Config::Trace); }

Logger::Logger(const Config& config, int level)
: config(config)
, level(level) {
    ss << config.genPrefix(level);
}

Logger::~Logger() {
    config.lock();
    config.doWrite(ss.str(), level);
    config.unlock();
}

} // namespace logging
} // namespace bl
