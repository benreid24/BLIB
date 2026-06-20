#ifndef BLIB_ENGINE_SCHEDULERS_HPP
#define BLIB_ENGINE_SCHEDULERS_HPP

#include <BLIB/Util/TaskScheduler.hpp>

namespace bl
{
namespace engine
{
class Engine;

/**
 * @brief Collection of task schedulers for the engine and its various thread pools
 *
 * @ingroup Engine
 */
struct Schedulers {
    /// Schedules tasks on the loop thread pool. Timing based on simulaed time
    util::TaskScheduler frameScheduler;

    /// Schedules tasks on the loop thread pool. Timing based on real time
    util::TaskScheduler frameSchedulerRealtime;

    /// Schedules tasks on the loop thread pool. Real time and executed even if not focused
    util::TaskScheduler frameSchedulerBackground;

    /// Schedules tasks on the fast  task thread pool. Timing based on simulaed time
    util::TaskScheduler fastPoolScheduler;

    /// Schedules tasks on the fast  task thread pool. Timing based on real time
    util::TaskScheduler fastPoolSchedulerRealtime;

    /// Schedules tasks on the fast  task thread pool. Real time and executed even if not focused
    util::TaskScheduler fastPoolSchedulerBackground;

    /// Schedules tasks on the slow  task thread pool. Timing based on simulaed time
    util::TaskScheduler slowPoolScheduler;

    /// Schedules tasks on the slow  task thread pool. Timing based on real time
    util::TaskScheduler slowPoolSchedulerRealtime;

    /// Schedules tasks on the slow  task thread pool. Real time and executed even if not focused
    util::TaskScheduler slowPoolSchedulerBackground;

    /**
     * @brief Creates the engine schedulers
     *
     * @param loopPool The thread pool for the main engine loop
     * @param fastPool The thread pool for fast background tasks
     * @param slowPool The thread pool for slow background tasks
     */
    Schedulers(util::ThreadPool& loopPool, util::ThreadPool& fastPool, util::ThreadPool& slowPool);

private:
    void update(float dt, float realDt);
    void updateBackground(float dt);

    friend class Engine;
};

} // namespace engine
} // namespace bl

#endif
