#ifndef BLIB_LOGGING_LOGGINGCONFIG_HPP
#define BLIB_LOGGING_LOGGINGCONFIG_HPP

#include <BENG/Util/NonCopyable.hpp>
#include <fstream>
#include <list>
#include <mutex>
#include <optional>
#include <ostream>
#include <string>
#include <vector>

namespace bg
{
class Logger;

/**
 * @brief Global log config class. Stores the various output streams and their configured log
 *        levels. Loggers flush their output here. Logging is threadsafe, however none of the
 *        public methods are threadsafe
 *
 * @ingroup Logging
 *
 */
class LoggingConfig : public NonCopyable {
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
     * @brief Set whether to log times in UTC or local time. Default is local time
     *
     * @param utc True to log in UTC, false for local time
     */
    static void timeInUTC(bool utc);

private:
    mutable std::mutex mutex;
    bool utc;
    mutable std::vector<std::pair<std::ostream*, int>> outputs;
    std::list<std::fstream> files;

    void doWrite(const std::string& content, int level) const;

    std::string genPrefix(int level) const;

    void lock() const;
    void unlock() const;

    LoggingConfig();
    static LoggingConfig& get();

    friend class Logger;
};

} // namespace bg

#endif