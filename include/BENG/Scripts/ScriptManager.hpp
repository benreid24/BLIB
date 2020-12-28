#ifndef BLIB_SCRIPTS_SCRIPTMANAGER_HPP
#define BLIB_SCRIPTS_SCRIPTMANAGER_HPP

#include <BENG/Scripts/Script.hpp>
#include <BENG/Util/NonCopyable.hpp>
#include <atomic>
#include <mutex>

namespace bg
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
     * @param timeout Time, in seconds, to wait reporting status
     *
     */
    bool terminateAll(float timeout = 5.0f);

private:
    std::mutex mutex;
    std::list<Script::ExecutionContext::WPtr> scripts;

    /**
     * @brief Registers a Script to have its execution tracked
     *
     * @param record The execution record to utilize
     */
    void watch(Script::ExecutionContext::WPtr record);

    /**
     * @brief Removes records of completed scripts
     *
     */
    void clean();

    friend class Script;
};

} // namespace bg

#endif