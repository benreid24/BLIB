#include <BLIB/Engine/Schedulers.hpp>

namespace bl
{
namespace engine
{
Schedulers::Schedulers(util::ThreadPool& loopPool, util::ThreadPool& fastPool,
                       util::ThreadPool& slowPool)
: frameScheduler(loopPool)
, frameSchedulerRealtime(loopPool)
, frameSchedulerBackground(loopPool)
, fastPoolScheduler(fastPool)
, fastPoolSchedulerRealtime(fastPool)
, fastPoolSchedulerBackground(fastPool)
, slowPoolScheduler(slowPool)
, slowPoolSchedulerRealtime(slowPool)
, slowPoolSchedulerBackground(slowPool) {}

void Schedulers::update(float dt, float realDt) {
    updateBackground(realDt);
    frameScheduler.update(dt);
    frameSchedulerRealtime.update(realDt);
    fastPoolScheduler.update(dt);
    fastPoolSchedulerRealtime.update(realDt);
    slowPoolScheduler.update(dt);
    slowPoolSchedulerRealtime.update(realDt);
}

void Schedulers::updateBackground(float dt) {
    frameSchedulerBackground.update(dt);
    fastPoolSchedulerBackground.update(dt);
    slowPoolSchedulerBackground.update(dt);
}

} // namespace engine
} // namespace bl
