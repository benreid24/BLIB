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
        workers.emplace_back(std::bind(&ThreadPool::worker, this, std::placeholders::_1));
    }

    return true;
}

bool ThreadPool::running() const { return !workers.empty() && !shuttingDown.load(); }

void ThreadPool::drain() {
    std::unique_lock lock(taskMutex);
    const auto isDrained = [this]() { return tasks.empty() && inFlightCount.load() == 0; };
    if (isDrained()) { return; }
    taskDoneCv.wait(lock, isDrained);
}

void ThreadPool::shutdown() {
    BL_LOG_INFO << "Thread pool shutting down";

    shuttingDown.store(true);
    drain();

    for (auto& t : workers) {
        if (!t.joinable()) { continue; }
        t.request_stop();
    }

    // give workers time to settle before notifying cv
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    taskQueuedCv.notify_all();

    for (auto& t : workers) {
        if (!t.joinable()) { continue; }
        t.join();
    }

    std::unique_lock lock(taskMutex);
    workers.clear();
    inFlightCount.store(0);
    shuttingDown.store(false);

    BL_LOG_INFO << "Thread pool shut down";
}

std::future<void> ThreadPool::queueTask(Task&& task) {
    if (shuttingDown.load() || !running()) { return {}; }

    std::unique_lock lock(taskMutex);
    if (shuttingDown.load()) { return {}; } // in case of race

    auto& t = tasks.emplace(std::forward<Task>(task));
    lock.unlock();
    taskQueuedCv.notify_one();

    return t.get_future();
}

void ThreadPool::worker(std::stop_token stopToken) {
    BL_LOG_INFO << "Worker thread started";

    while (!stopToken.stop_requested()) {
        std::unique_lock lock(taskMutex);

        if (stopToken.stop_requested()) { break; }

        if (tasks.empty()) { taskQueuedCv.wait(lock); }

        if (stopToken.stop_requested()) { break; }
        if (tasks.empty()) {
            BL_LOG_WARN << "No tasks for worker after waiting, terminating worker";
            break;
        }

        std::packaged_task<void()> task = std::move(tasks.front());
        ++inFlightCount;
        tasks.pop();
        lock.unlock();
        task();
        --inFlightCount;
        taskDoneCv.notify_all();
    }

    BL_LOG_INFO << "Worker thread terminated";
}

} // namespace util
} // namespace bl
