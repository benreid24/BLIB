#ifndef BLIB_SCRIPTS_SCRIPTMANAGER_HPP
#define BLIB_SCRIPTS_SCRIPTMANAGER_HPP

#include <BLIB/Scripts/Script.hpp>
#include <BLIB/Util/NonCopyable.hpp>
#include <atomic>
#include <mutex>

namespace bl
{
/**
 * @brief Global singleton to manage conurrently running Scripts
 *
 */
class ScriptManager : private NonCopyable {
public:
    /**
     * @brief Returns the global ScriptManager
     *
     */
    static ScriptManager& get();

    /**
     * @brief Terminates all running scripts
     *
     * @param timeout Time, in seconds, to wait before force killing threads
     *
     */
    void terminateAll(float timeout = 5.0f);

    /**
     * @brief Registers a Script to have its execution tracked
     *
     * @param record The execution record to utilize
     */
    void watch(Script::ExecutionContext::WPtr record);

private:
    std::mutex mutex;
    std::list<Script::ExecutionContext::WPtr> scripts;

    ScriptManager() = default;
};

} // namespace bl

#endif