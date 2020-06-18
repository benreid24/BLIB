#include <BLIB/Logging.hpp>

#include <ctime>
#include <iomanip>
#include <iostream>
#include <sstream>

namespace bl
{
namespace
{
const char* levels[] = {"TRACE", "DEBUG", "INFO", "WARN", "ERROR", "CRITICAL"};
} // namespace

LoggingConfig::LoggingConfig()
: utc(false) {}

LoggingConfig& LoggingConfig::get() {
    static LoggingConfig config;
    return config;
}

void LoggingConfig::configureOutput(std::ostream& s, int level) {
    LoggingConfig& c = get();
    for (auto& o : c.outputs) {
        if (o.first == &s) {
            o.second = level;
            return;
        }
    }
    c.outputs.push_back(std::make_pair(&s, level));
}

void LoggingConfig::addFileOutput(const std::string& file, int level) {
    LoggingConfig& c = get();
    c.files.emplace_back(file.c_str(), std::ios::out | std::ios::app);
    c.files.back() << std::endl << "Beginning new log" << std::endl;
    configureOutput(c.files.back(), level);
}

void LoggingConfig::timeInUTC(bool utc) { get().utc = utc; }

std::string LoggingConfig::genPrefix(int level) const {
    if (outputs.empty()) outputs.push_back(std::make_pair(&std::cout, Info));

    std::stringstream ss;
    const std::time_t now = std::time(nullptr);
    const auto time       = utc ? *std::gmtime(&now) : *std::localtime(&now);
    ss << std::setfill('0') << std::setw(4) << (time.tm_year + 1900) << "-"
       << std::setfill('0') << std::setw(2) << time.tm_mon << "-" << std::setfill('0')
       << std::setw(2) << time.tm_mday << "T";
    ss << std::setfill('0') << std::setw(2) << time.tm_hour << ":" << std::setfill('0')
       << std::setw(2) << time.tm_min << ":" << std::setfill('0') << std::setw(2)
       << time.tm_sec << " ";

    ss << levels[level] << " ";
    return ss.str();
}

void LoggingConfig::doWrite(const std::string& data, int level) const {
    for (const auto& log : outputs) {
        if (log.second <= level) (*log.first) << data << std::endl;
    }
}

void LoggingConfig::lock() const { mutex.lock(); }

void LoggingConfig::unlock() const { mutex.unlock(); }

} // namespace bl