#ifndef BLIB_SCRIPTS_SCRIPTMANAGER_HPP
#define BLIB_SCRIPTS_SCRIPTMANAGER_HPP

#include <BLIB/Scripts/Script.hpp>
#include <BLIB/Util/NonCopyable.hpp>
#include <atomic>
#include <mutex>

namespace bl
{
namespace engine
{
class Engine;
}

namespace script
{
namespace unittest
{
class ManagerOwner;
}

/**
 * @brief Utility class that can manage sets of concurrently running scripts
 *
 */
class Manager : private util::NonCopyable {
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

    Manager() = default;

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
    friend class engine::Engine;
    friend class unittest::ManagerOwner;
};

} // namespace script
} // namespace bl

#endif
