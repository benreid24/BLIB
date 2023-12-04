#define _CRT_SECURE_NO_WARNINGS

#include <BLIB/Logging.hpp>

#include <BLIB/Util/FileUtil.hpp>
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

std::string logName(const std::string& base, int i) {
    std::stringstream ss;
    ss << base << "." << i << ".log";
    return ss.str();
}

} // namespace

Config::Config()
: utc(false) {
    outputs.reserve(4);
    outputs.emplace_back(&std::cout, Info);
}

Config& Config::get() {
    // yes this is a "leak" but we never want to destruct this, otherwise we cannot
    // log in static/global destructors
    static Config* config = new Config();
    return *config;
}

void Config::rollLogs(const std::string& path, const std::string& name, unsigned int n) {
    std::unique_lock lock(get().mutex);

    util::FileUtil::createDirectory(path);

    const std::string base = util::FileUtil::joinPath(path, name);
    for (int i = n - 1; i > 1; --i) {
        const std::string src = logName(base, i);
        if (util::FileUtil::exists(src)) { util::FileUtil::copyFile(src, logName(base, i + 1)); }
    }
    const std::string og = base + ".log";
    if (util::FileUtil::exists(og)) { util::FileUtil::copyFile(og, logName(base, 2)); }
}

void Config::configureOutput(std::ostream& s, int level) {
    std::unique_lock lock(get().mutex);
    configureOutputLocked(s, level);
}

void Config::configureOutputLocked(std::ostream& s, int level) {
    Config& c = get();
    for (auto& o : c.outputs) {
        if (o.first == &s) {
            o.second = level;
            return;
        }
    }
    c.outputs.emplace_back(&s, level);
}

void Config::addFileOutput(const std::string& file, int level) {
    std::unique_lock lock(get().mutex);

    Config& c = get();
    c.files.emplace_back(file.c_str(), std::ios::out | std::ios::app);
    c.files.back() << std::endl << "Beginning new log" << std::endl;
    configureOutputLocked(c.files.back(), level);
}

void Config::timeInUTC(bool utc) { get().utc = utc; }

std::string Config::genPrefix(int level) const {
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

void Config::doWrite(const std::string& data, int level) {
    std::unique_lock lock(mutex);
    for (const auto& log : outputs) {
        if (log.second <= level) { (*log.first) << data << std::flush; }
    }
}

} // namespace logging
} // namespace bl
