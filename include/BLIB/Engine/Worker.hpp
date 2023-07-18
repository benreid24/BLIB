#ifndef BLIB_ENGINE_WORKER_HPP
#define BLIB_ENGINE_WORKER_HPP

#include <BLIB/Containers/RingQueue.hpp>
#include <BLIB/Util/NonCopyable.hpp>
#include <atomic>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <thread>
#include <type_traits>

namespace bl
{
namespace engine
{
class Engine;

/**
 * @brief Worker interface providing access to the background worker thread of the engine. This
 *        thread performs logging and other async engine tasks that are relatively lightweight and
 *        sporadic. The thread is terminated when the engine instance is destructed.
 *
 * @ingroup Engine
 *
 */
class Worker : private util::NonCopyable {
public:
    /// @brief Represents a task that is queued
    using QueuedTask = std::function<void()>;

    /**
     * @brief Submit a task to be executed. Submitted tasks should be relatively lightweight. More
     *        advanced or intensive workflows should get their own thread generally. If there is no
     *        active worker thread then the task is executed by the calling thread immediately
     *
     * @tparam Task The task type to submit
     * @param task The task to submit.
     */
    template<typename Task>
    static void submit(Task&& task);

private:
    static Worker* worker;

    std::atomic_bool running;
    ctr::RingQueue<QueuedTask> workQueue;
    std::mutex mutex;
    std::condition_variable cv;
    std::thread runner;

    Worker();
    ~Worker();
    void runLoop();

    friend class Engine;
};

///////////////////////////// INLINE FUNCTIONS ////////////////////////////////////

template<typename T>
void Worker::submit(T&& task) {
    static_assert(std::is_invocable_v<T>,
                  "submit() only accepts callables with the signature void()");

    if (worker) {
        std::unique_lock lock(worker->mutex);
        if (worker) {
            worker->workQueue.emplace(std::forward<T>(task));
            lock.unlock();
            worker->cv.notify_all();
            return;
        }
    }

    task();
}

} // namespace engine
} // namespace bl

#endif
