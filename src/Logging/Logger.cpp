#include <BLIB/Logging.hpp>

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
: level(level)
, time(std::time(nullptr)) {}

Logger::~Logger() {
    if (ss.str().back() != '\n') { ss << '\n'; }
    Config::get().doWrite(time, ss.str(), level);
}

} // namespace logging
} // namespace bl
