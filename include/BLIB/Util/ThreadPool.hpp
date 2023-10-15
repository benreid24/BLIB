#ifndef BLIB_UTIL_THREADPOOL_HPP
#define BLIB_UTIL_THREADPOOL_HPP

#include <BLIB/Util/NonCopyable.hpp>
#include <atomic>
#include <condition_variable>
#include <functional>
#include <future>
#include <list>
#include <mutex>
#include <queue>
#include <shared_mutex>
#include <thread>

namespace bl
{
namespace util
{
/**
 * @brief Provides a thread pool with variable concurrency and support for draining
 *
 * @ingroup Util
 */
class ThreadPool : private NonCopyable {
public:
    /// Callback signature of tasks to be submitted
    using Task = std::function<void()>;

    /**
     * @brief Initializes the thread pool. Does not start it
     */
    ThreadPool();

    /**
     * @brief Terminates the pool
     */
    ~ThreadPool();

    /**
     * @brief Starts the thread pool with the given concurrency. If the workerCount is 0 then
     *        std::thread::hardware_concurrency() is used. If that fails then 4 is used
     *
     * @param workerCount The number of workers to start
     * @return True if the pool was started, false if the pool was already running
     */
    bool start(unsigned int workerCount = 0);

    /**
     * @brief Returns whether or not the pool is running
     */
    bool running() const;

    /**
     * @brief Blocks until the task queue is empty and all workers are idle. If more tasks are
     *        submitted during this call it will continue to block until those are completed as well
     */
    void drain();

    /**
     * @brief Prevents new tasks from being submitted, drains the queue, and stops all workers
     */
    void shutdown();

    /**
     * @brief Queues a task to be executed by a worker. Returns an invalid future if the pool is not
     *        accepting tasks
     *
     * @param task The task to execute
     * @return A future that can be used to wait on the task if it was submitted
     */
    std::future<void> queueTask(Task&& task);

private:
    std::atomic_bool shuttingDown;
    std::list<std::thread> workers;
    std::queue<std::packaged_task<void()>> tasks;
    std::atomic_uint32_t inFlightCount;
    std::mutex taskMutex;
    std::condition_variable taskQueuedCv;
    std::condition_variable_any taskDoneCv;

    void worker();
};

} // namespace util
} // namespace bl

#endif
