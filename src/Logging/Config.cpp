#include <BLIB/Logging.hpp>

#include <ctime>
#include <iomanip>
#include <iostream>
#include <sstream>

namespace bl
{
namespace logging
{
namespace
{
const char* levels[] = {"TRACE", "DEBUG", "INFO", "WARN", "ERROR", "CRITICAL"};
} // namespace

Config::Config()
: utc(false) {}

Config& Config::get() {
    // yes this is a "leak" but we never want to destruct this, otherwise we cannot
    // log in static/global destructors
    static Config* config = new Config();
    return *config;
}

void Config::configureOutput(std::ostream& s, int level) {
    Config& c = get();
    for (auto& o : c.outputs) {
        if (o.first == &s) {
            o.second = level;
            return;
        }
    }
    c.outputs.push_back(std::make_pair(&s, level));
}

void Config::addFileOutput(const std::string& file, int level) {
    Config& c = get();
    c.files.emplace_back(file.c_str(), std::ios::out | std::ios::app);
    c.files.back() << std::endl << "Beginning new log" << std::endl;
    configureOutput(c.files.back(), level);
}

void Config::timeInUTC(bool utc) { get().utc = utc; }

std::string Config::genPrefix(int level) const {
    if (outputs.empty()) outputs.push_back(std::make_pair(&std::cout, Info));

    std::stringstream ss;
    const std::time_t now = std::time(nullptr);
    const auto time       = utc ? *std::gmtime(&now) : *std::localtime(&now);
    ss << std::setfill('0') << std::setw(4) << (time.tm_year + 1900) << "-" << std::setfill('0')
       << std::setw(2) << (time.tm_mon + 1) << "-" << std::setfill('0') << std::setw(2)
       << time.tm_mday << "T";
    ss << std::setfill('0') << std::setw(2) << time.tm_hour << ":" << std::setfill('0')
       << std::setw(2) << time.tm_min << ":" << std::setfill('0') << std::setw(2) << time.tm_sec
       << " ";

    ss << levels[level] << " ";
    return ss.str();
}

void Config::doWrite(const std::string& data, int level) const {
    for (const auto& log : outputs) {
        if (log.second <= level) (*log.first) << data << std::endl;
    }
}

void Config::lock() const { mutex.lock(); }

void Config::unlock() const { mutex.unlock(); }

} // namespace logging
} // namespace bl
