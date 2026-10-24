#ifndef BLIB_SYSTEMS_JOLT_JOBSYSTEM_HPP
#define BLIB_SYSTEMS_JOLT_JOBSYSTEM_HPP

#include <Jolt/Jolt.h>

#include <BLIB/Containers/ObjectPool.hpp>
#include <BLIB/Util/ThreadPool.hpp>
#include <Jolt/Core/JobSystemWithBarrier.h>

namespace bl
{
namespace sys
{
/// Collection of integration helpers for Jolt physics
namespace jolt
{
/**
 * @brief Job system for Jolt physics that uses the engine thread pool
 *
 * @ingroup Systems
 */
class JobSystem : public JPH::JobSystemWithBarrier {
public:
    static constexpr unsigned int MaxActiveJobs = 2048;
    static constexpr unsigned int MaxBarriers   = 8;

    /**
     * @brief Creates the job system
     *
     * @param threadPool The engine threadpool
     */
    JobSystem(util::ThreadPool& threadPool);

    /**
     * @brief Destroys the job system
     */
    virtual ~JobSystem() = default;

private:
    std::mutex poolMutex;
    ctr::ObjectPool<Job> jobs;
    util::ThreadPool& threadPool;

    int GetMaxConcurrency() const override;
    JobHandle CreateJob(const char* inName, JPH::ColorArg inColor, const JobFunction& inJobFunction,
                        std::uint32_t inNumDependencies) override;
    void QueueJob(Job* inJob) override;
    void QueueJobs(Job** inJobs, std::uint32_t inNumJobs) override;
    void FreeJob(Job* inJob) override;
};

} // namespace jolt
} // namespace sys
} // namespace bl

#endif
