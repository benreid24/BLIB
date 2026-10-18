#ifndef BLIB_UTIL_RAIICLEANUP_HPP
#define BLIB_UTIL_RAIICLEANUP_HPP

#include <BLIB/Util/NonCopyable.hpp>
#include <functional>

namespace bl
{
namespace util
{
/**
 * @brief Helper for executing automatic cleanup tasks inside a scope. Use in methods that allocate
 *        and need different cleanup based on progress before failure
 *
 * @ingroup Util
 */
class RAIICleanup : private NonCopyable {
public:
    /**
     * @brief Creates the helper with a hint of how many tasks will be added
     *
     * @param hint The number of tasks to reserve memory for
     */
    RAIICleanup(unsigned int hint = 4);

    /**
     * @brief Runs the cleanup tasks unless disarmed
     */
    ~RAIICleanup();

    /**
     * @brief Adds a task to be ran on destruct. Tasks are executed in reverse order of addition
     *
     * @param task The task to run on cleanup
     */
    void add(std::function<void()> task);

    /**
     * @brief Cancels cleanup tasks from running on destruct
     */
    void disarm();

private:
    std::vector<std::function<void()>> tasks;
    bool armed;
};

} // namespace util
} // namespace bl

#endif
