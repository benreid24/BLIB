#include <BLIB/Logging.hpp>

#include <BLIB/Engine/Worker.hpp>

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
    engine::Worker::submit(std::bind(&Config::doWrite, &Config::get(), ss.str(), level));
}

} // namespace logging
} // namespace bl
