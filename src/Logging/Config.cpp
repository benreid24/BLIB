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
: utc(false)
, bufferIndex(0) {
    outputs.reserve(4);
    outputs.emplace_back(&std::cout, Info);
    buffering.clear();
}

Config::Buffer::Buffer()
: head(0) {
    lines.resize(Config::DefaultBufferSize);
}

Config& Config::get() {
    // yes this is a "leak" but we never want to destruct this, otherwise we cannot
    // log in static/global destructors
    static Config* config = new Config();
    return *config;
}

void Config::rollLogs(const std::string& path, const std::string& name, unsigned int n) {
    std::unique_lock lock(get().outputMutex);

    util::FileUtil::createDirectory(path);

    const std::string base = util::FileUtil::joinPath(path, name);
    for (int i = n - 1; i > 1; --i) {
        const std::string src = logName(base, i);
        if (util::FileUtil::exists(src)) { util::FileUtil::copyFile(src, logName(base, i + 1)); }
    }
    const std::string og = base + ".log";
    if (util::FileUtil::exists(og)) { util::FileUtil::copyFile(og, logName(base, 2)); }
}

void Config::setBufferSize(std::size_t lines) {
    get().buffering.clear();
    std::unique_lock lock(get().outputMutex);
    for (Buffer& b : get().buffers) {
        std::unique_lock flushLock(b.flushMutex);
        b.lines.resize(lines);
    }
    get().buffering.test_and_set();
}

void Config::setIsBuffering(bool b) {
    Config& cfg = get();
    if (b) { cfg.buffering.test_and_set(); }
    else {
        cfg.buffering.clear();
        const unsigned int bi = cfg.bufferIndex.load();
        cfg.flushBuffer(bi);
        cfg.flushBuffer((bi + 1) % 2);
    }
}

void Config::flush() {
    Config& cfg = get();
    if (!cfg.buffering.test()) { return; }

    const unsigned int bi = cfg.bufferIndex.fetch_xor(1);
    cfg.flushBuffer(bi);
}

void Config::configureOutput(std::ostream& s, int level) {
    std::unique_lock lock(get().outputMutex);
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
    std::unique_lock lock(get().outputMutex);

    Config& c = get();
    c.files.emplace_back(file.c_str(), std::ios::out | std::ios::app);
    c.files.back() << std::endl << "Beginning new log" << std::endl;
    configureOutputLocked(c.files.back(), level);
}

void Config::timeInUTC(bool utc) { get().utc = utc; }

void Config::doWrite(std::time_t time, std::string&& data, int level) {
    if (!buffering.test()) {
        std::unique_lock lock(outputMutex);
        outputLineLocked(data, level, time, true);
        return;
    }

    const unsigned int bi = bufferIndex.load();
    Buffer* buffer        = &buffers[bi];
    unsigned int head     = buffer->head.fetch_add(1);
    bool needsFlush       = false;

    if (head == buffer->lines.size()) {
        bufferIndex.store((bi + 1) % 2);
        needsFlush = true;
    }
    if (head >= buffer->lines.size()) {
        // current buffer is being flushed, write our line to the other buffer
        buffer = &buffers[(bi + 1) % 2];
        head   = buffer->head.fetch_add(1);
        if (head >= buffer->lines.size()) {
            std::unique_lock lock(outputMutex);
            outputLineLocked("Both log buffers full, increase buffer size or flush frequency",
                             Critical,
                             std::time(nullptr),
                             false);
            outputLineLocked(data, level, time, true);
            return;
        }
    }

    buffer->lines[head].assign(std::move(data), level, time);

    if (needsFlush) { flushBuffer(bi); }
}

void Config::outputLineLocked(std::string_view content, int level, std::time_t time, bool flush) {
    const auto tm = utc ? *std::gmtime(&time) : *std::localtime(&time);

    for (const auto& log : outputs) {
        if (log.second <= level) {
            const char f = log.first->fill();
            log.first->fill('0');

            (*log.first) << std::setw(4) << (tm.tm_year + 1900) << "-" << std::setw(2)
                         << (tm.tm_mon + 1) << "-" << std::setw(2) << tm.tm_mday << "T";
            (*log.first) << std::setw(2) << tm.tm_hour << ":" << std::setw(2) << tm.tm_min << ":"
                         << std::setw(2) << tm.tm_sec << " ";

            (*log.first) << levels[level] << " ";

            (*log.first) << content;

            log.first->fill(f);
            if (flush) { (*log.first) << std::flush; }
        }
    }
}

void Config::flushBuffer(unsigned int i) {
    Buffer& buffer = buffers[i];
    std::unique_lock outputLock(outputMutex);
    std::unique_lock flushLock(buffer.flushMutex);

    const unsigned int lineCount = buffer.head.load();
    if (lineCount == 0) { return; }

    for (unsigned int i = 0; i < lineCount; ++i) {
        Line& line = buffer.lines[i];
        outputLineLocked(line.content, line.level, line.time, false);
    }
    buffer.head.store(0);
    for (const auto& log : outputs) { (*log.first) << std::flush; }
}

void logging::Config::Line::assign(std::string&& content, int level, std::time_t time) {
    this->content = std::move(content);
    this->level   = level;
    this->time    = time;
}

} // namespace logging
} // namespace bl
