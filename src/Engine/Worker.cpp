#include <BLIB/Engine/Worker.hpp>

#include <BLIB/Logging.hpp>

namespace bl
{
namespace engine
{
Worker* Worker::worker = nullptr;

Worker::Worker()
: running(true)
, workQueue(64)
, runner(&Worker::runLoop, this) {
    std::unique_lock lock(mutex);
    worker = this;
    lock.unlock();
    BL_LOG_INFO << "Started engine worker";
}

Worker::~Worker() {
    BL_LOG_INFO << "Shutting down engine worker";
    mutex.lock();
    worker  = nullptr;
    running = false;
    cv.notify_all();
    mutex.unlock();
    runner.join();
    BL_LOG_INFO << "Engine worker shutdown";
}

void Worker::runLoop() {
    while (running) {
        std::unique_lock lock(mutex);

        if (workQueue.empty()) { cv.wait(lock); }

        while (!workQueue.empty()) {
            workQueue.front()();
            workQueue.pop();
        }
    }
}

} // namespace engine
} // namespace bl
