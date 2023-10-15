#include <BLIB/Util/ThreadPool.hpp>

#include <BLIB/Logging.hpp>
#include <cmath>

namespace bl
{
namespace util
{
ThreadPool::ThreadPool()
: shuttingDown(false)
, inFlightCount(0) {}

ThreadPool::~ThreadPool() { shutdown(); }

bool ThreadPool::start(unsigned int wc) {
    if (running()) { return false; }
    if (wc == 0) { wc = std::max(std::jthread::hardware_concurrency(), 4u); }

    BL_LOG_INFO << "Thread pool starting with " << wc << " workers";

    for (unsigned int i = 0; i < wc; ++i) {
        workers.emplace_back(std::bind(&ThreadPool::worker, this));
    }

    return true;
}

bool ThreadPool::running() const { return !workers.empty() && !shuttingDown.load(); }

void ThreadPool::drain() {
    std::unique_lock lock(taskMutex);
    const auto isDrained = [this]() { return tasks.empty() && inFlightCount.load() == 0; };
    if (!isDrained()) { taskDoneCv.wait(lock, isDrained); }
}

void ThreadPool::shutdown() {
    BL_LOG_INFO << "Thread pool shutting down";

    shuttingDown.store(true);
    taskQueuedCv.notify_all();

    for (auto& t : workers) {
        if (!t.joinable()) { continue; }
        t.join();
    }

    std::unique_lock lock(taskMutex);
    workers.clear();
    inFlightCount.store(0);

    // run remaining tasks serially
    while (!tasks.empty()) {
        tasks.front()();
        tasks.pop();
    }

    shuttingDown.store(false);

    BL_LOG_INFO << "Thread pool shut down";
}

std::future<void> ThreadPool::queueTask(Task&& task) {
    if (shuttingDown.load() || !running()) { return {}; }

    std::unique_lock lock(taskMutex);
    if (shuttingDown.load()) { return {}; } // in case of race

    auto& t = tasks.emplace(std::forward<Task>(task));
    auto f  = t.get_future();
    lock.unlock();
    taskQueuedCv.notify_one();

    return f;
}

void ThreadPool::worker() {
    BL_LOG_INFO << "Worker thread started";

    while (!shuttingDown) {
        std::unique_lock lock(taskMutex);

        if (shuttingDown) { break; }

        if (tasks.empty()) { taskQueuedCv.wait(lock); }

        if (tasks.empty()) { continue; }

        std::packaged_task<void()> task = std::move(tasks.front());
        ++inFlightCount;
        tasks.pop();
        lock.unlock();
        task();
        lock.lock();
        --inFlightCount;
        taskDoneCv.notify_all();

        if (shuttingDown) { break; }
    }

    BL_LOG_INFO << "Worker thread terminated";
}

} // namespace util
} // namespace bl
