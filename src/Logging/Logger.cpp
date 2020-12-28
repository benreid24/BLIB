#include <BENG/Logging.hpp>

namespace bg
{
Logger Logger::critical() { return Logger(LoggingConfig::get(), LoggingConfig::Critical); }

Logger Logger::error() { return Logger(LoggingConfig::get(), LoggingConfig::Error); }

Logger Logger::warn() { return Logger(LoggingConfig::get(), LoggingConfig::Warn); }

Logger Logger::info() { return Logger(LoggingConfig::get(), LoggingConfig::Info); }

Logger Logger::debug() { return Logger(LoggingConfig::get(), LoggingConfig::Debug); }

Logger Logger::trace() { return Logger(LoggingConfig::get(), LoggingConfig::Trace); }

Logger::Logger(const LoggingConfig& config, int level)
: config(config)
, level(level) {
    ss << config.genPrefix(level);
}

Logger::~Logger() {
    config.lock();
    config.doWrite(ss.str(), level);
    config.unlock();
}

} // namespace bg