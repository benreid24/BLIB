#ifndef BLIB_LOGGING_LOGGINGCONFIG_HPP
#define BLIB_LOGGING_LOGGINGCONFIG_HPP

#include <BLIB/Util/NonCopyable.hpp>
#include <atomic>
#include <fstream>
#include <list>
#include <mutex>
#include <ostream>
#include <string>
#include <vector>

namespace bl
{
namespace logging
{
struct Logger;

/**
 * @brief Global log config class. Stores the various output streams and their configured log
 *        levels. Loggers flush their output here. Logging is threadsafe, however none of the
 *        public methods are threadsafe
 *
 * @ingroup Logging
 *
 */
class Config : public util::NonCopyable {
public:
    static constexpr std::size_t DefaultBufferSize = 1024;

    static constexpr int Critical = 5;
    static constexpr int Error    = 4;
    static constexpr int Warn     = 3;
    static constexpr int Info     = 2;
    static constexpr int Debug    = 1;
    static constexpr int Trace    = 0;

    /**
     * @brief Set the size of buffers used to store logs lines before they are flushed. Two buffers
     *
     * @param size The size of the buffer in log lines
     */
    static void setBufferSize(std::size_t lines);

    /**
     * @brief Sets whether outputs are buffered or immediately written to all streams. If buffering
     *        is enabled then call flush() periodically (called by Engine when running)
     *
     * @param buffering True to buffer, false to write immediately
     */
    static void setIsBuffering(bool buffering);

    /**
     * @brief Flushes buffered outputs. No effect if not buffering
     */
    static void flush();

    /**
     * @brief Add or modify an output stream to the config. Can be called multiple times on the
     *        same stream to change the log level. If no output is configured before the first
     *        item logged the default is to log to std::cout at Info level
     *
     * @param output The stream to log to. Must remain valid
     * @param logLevel The level at which to send logs. Lower level logs omitted
     */
    static void configureOutput(std::ostream& output, int logLevel = Info);

    /**
     * @brief Helper method to open a file stream and configure it's level. The config
     *        maintains the stream internally, however the log level may not be changed later
     *
     * @param file Filename to open and log to. Logs are appended
     * @param logLevel The level at which to send logs. Lower level logs omitted
     */
    static void addFileOutput(const std::string& file, int logLevel = Info);

    /**
     * @brief Rolls logs in the given directory. Files in the form "{name}.log" are rolled into
     *        "{name}.{n}.log" and logs of the form "{name}.{n}.log" are rolled into
     *        "{name}.{n+1}.log". Logs where n >= keepCount are deleted
     *
     * @param dir The directory to roll logs in
     * @param name The base filename of the log files to roll, excluding extension
     * @param keepCount How many logs to keep, including the current log file
     */
    static void rollLogs(const std::string& dir, const std::string& name,
                         unsigned int keepCount = 3);

    /**
     * @brief Set whether to log times in UTC or local time. Default is local time
     *
     * @param utc True to log in UTC, false for local time
     */
    static void timeInUTC(bool utc);

private:
    struct Line {
        std::string content;
        int level;
        std::time_t time;

        void assign(std::string&& content, int level, std::time_t time);
    };

    struct Buffer {
        std::mutex flushMutex;
        std::vector<Line> lines;
        std::atomic<unsigned int> head;

        Buffer();
    };

    std::mutex outputMutex;
    bool utc;
    std::vector<std::pair<std::ostream*, int>> outputs;
    std::list<std::fstream> files;

    std::atomic_flag buffering;
    Buffer buffers[2];
    std::atomic<unsigned int> bufferIndex;

    void doWrite(std::time_t time, std::string&& content, int level);
    static void configureOutputLocked(std::ostream& output, int logLevel);
    void flushBuffer(unsigned int i);
    void outputLineLocked(std::string_view content, int level, std::time_t time, bool flush);

    Config();
    static Config& get();

    friend struct Logger;
};

} // namespace logging
} // namespace bl

#endif
