#ifndef BLIB_LOGGING_LOGGINGCONFIG_HPP
#define BLIB_LOGGING_LOGGINGCONFIG_HPP

#include <BLIB/Util/NonCopyable.hpp>
#include <fstream>
#include <list>
#include <mutex>
#include <optional>
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
    static constexpr int Critical = 5;
    static constexpr int Error    = 4;
    static constexpr int Warn     = 3;
    static constexpr int Info     = 2;
    static constexpr int Debug    = 1;
    static constexpr int Trace    = 0;

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
    std::mutex mutex;
    bool utc;
    std::vector<std::pair<std::ostream*, int>> outputs;
    std::list<std::fstream> files;

    void doWrite(const std::string& content, int level);
    std::string genPrefix(int level) const;
    static void configureOutputLocked(std::ostream& output, int logLevel);

    Config();
    static Config& get();

    friend struct Logger;
};

} // namespace logging
} // namespace bl

#endif
