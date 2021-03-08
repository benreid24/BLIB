#ifndef BLIB_LOGGING_LOGGER_HPP
#define BLIB_LOGGING_LOGGER_HPP

#include <BLIB/Logging/LoggingConfig.hpp>
#include <BLIB/Util/NonCopyable.hpp>
#include <cstring>
#include <sstream>

namespace bl
{
/**
 * @brief Actual logging class. Provides several static methods to begin log output. Instances
 *        of Logger only live long enough to build the log line and flush it via the
 *        LoggerConfig singleton instance
 *
 * @ingroup Logging
 *
 */
struct Logger : public util::NonCopyable {
    /**
     * @brief Flushes the generated log line
     *
     */
    ~Logger();

    /**
     * @brief Generic output operator. Any data type T that can be outputted to a std::ostream
     *        can be outputted here
     *
     * @param data The object to log
     * @return const Logger& This logger
     */
    template<typename T>
    const Logger& operator<<(const T& data) const;

    /**
     * @brief Create a new Logger for logging a critical error
     *
     */
    static Logger critical();

    /**
     * @brief Create a new Logger for logging an error
     *
     */
    static Logger error();

    /**
     * @brief Create a new Logger for logging a warning
     *
     */
    static Logger warn();

    /**
     * @brief Create a new Logger for logging a piece of info
     *
     */
    static Logger info();

    /**
     * @brief Create a new Logger for logging low level debug info
     *
     */
    static Logger debug();

    /**
     * @brief Create a new Logger for logging high volume low level info
     *
     */
    static Logger trace();

private:
    Logger(const LoggingConfig& config, int level);

    const LoggingConfig& config;
    const int level;
    mutable std::stringstream ss;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////
template<typename T>
const Logger& Logger::operator<<(const T& data) const {
    ss << data;
    return *this;
}

} // namespace bl

////////////////////////////////// MACROS //////////////////////////////////////

// clang-format off
#define __FILENAME__ (std::strrchr(__FILE__, '/') ? std::strrchr(__FILE__, '/') + 1 : __FILE__)

/// @brief Helper macro to log at critical level and include file, function, and line information
#define BL_LOG_CRITICAL bl::Logger::critical() << __FILENAME__ << " " << __FUNCTION__ << "()" << ":" << __LINE__ << " - "

/// @brief Helper macro to log at error level and include file, function, and line information
#define BL_LOG_ERROR bl::Logger::error() << __FILENAME__ << " " << __FUNCTION__ << "()" << ":" << __LINE__ << " - "

/// @brief Helper macro to log at warn level and include file, function, and line information
#define BL_LOG_WARN bl::Logger::warn() << __FILENAME__ << " " << __FUNCTION__ << "()" << ":" << __LINE__ << " - "

/// @brief Helper macro to log at info level and include file, function, and line information
#define BL_LOG_INFO bl::Logger::info() << __FILENAME__ << " " << __FUNCTION__ << "()" << ":" << __LINE__ << " - "

/// @brief Helper macro to log at debug level and include file, function, and line information
#define BL_LOG_DEBUG bl::Logger::debug() << __FILENAME__ << " " << __FUNCTION__ << "()" << ":" << __LINE__ << " - "

/// @brief Helper macro to log at trace level and include file, function, and line information
#define BL_LOG_TRACE bl::Logger::trace() << __FILENAME__ << " " << __FUNCTION__ << "()" << ":" << __LINE__ << " - "
// clang-format on

#endif