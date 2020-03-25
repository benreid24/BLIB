#ifndef BLIB_SCRIPTS_SCRIPTMANAGER_HPP
#define BLIB_SCRIPTS_SCRIPTMANAGER_HPP

#include <BLIB/Scripts/Script.hpp>
#include <BLIB/Util/NonCopyable.hpp>
#include <atomic>
#include <mutex>

namespace bl
{
/**
 * @brief Utility class that can manage sets of concurrently running scripts
 *
 */
class ScriptManager : private NonCopyable {
public:
    /**
     * @brief Terminates all running scripts
     *
     * @param timeout Time, in seconds, to wait before force killing threads
     *
     */
    void terminateAll(float timeout = 5.0f);

private:
    std::mutex mutex;
    std::list<Script::ExecutionContext::WPtr> scripts;

    /**
     * @brief Registers a Script to have its execution tracked
     *
     * @param record The execution record to utilize
     */
    void watch(Script::ExecutionContext::WPtr record);

    friend class Script;
};

} // namespace bl

#endif