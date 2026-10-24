#include <BLIB/Systems/Jolt/JobSystem.hpp>

namespace bl
{
namespace sys
{
namespace jolt
{
JobSystem::JobSystem(util::ThreadPool& threadPool)
: jobs(ctr::ObjectPool<Job>::GrowthPolicy::FailContinue, MaxActiveJobs)
, threadPool(threadPool) {
    JobSystemWithBarrier::Init(MaxBarriers);
}

int JobSystem::GetMaxConcurrency() const { return threadPool.threadCount(); }

JobSystem::JobHandle JobSystem::CreateJob(const char* inName, JPH::ColorArg inColor,
                                          const JobFunction& inJobFunction,
                                          std::uint32_t inNumDependencies) {
    ctr::ObjectPool<Job>::Iterator it = jobs.end();
    do {
        std::unique_lock lock(poolMutex);
        it = jobs.emplace(inName, inColor, this, inJobFunction, inNumDependencies);
        if (it == jobs.end()) {
            lock.unlock();
            std::this_thread::sleep_for(std::chrono::microseconds(100));
        }
    } while (it == jobs.end());

    JobHandle handle(&*it);

    if (inNumDependencies == 0) { QueueJob(&*it); }

    return handle;
}

void JobSystem::QueueJob(Job* inJob) {
    threadPool.queueTask([job = inJob]() {
        job->Execute();
        job->Release();
    });
}

void JobSystem::QueueJobs(Job** inJobs, std::uint32_t inNumJobs) {
    for (std::uint32_t i = 0; i < inNumJobs; ++i) { QueueJob(inJobs[i]); }
}

void JobSystem::FreeJob(Job* inJob) {
    const auto it = jobs.find(*inJob);
    if (it != jobs.end()) {
        std::unique_lock lock(poolMutex);
        jobs.erase(it);
    }
}

} // namespace jolt
} // namespace sys
} // namespace bl
