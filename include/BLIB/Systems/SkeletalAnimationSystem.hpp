#ifndef BLIB_SYSTEMS_SKELETALANIMATIONSYSTEM_HPP
#define BLIB_SYSTEMS_SKELETALANIMATIONSYSTEM_HPP

#include <BLIB/Components/Skeleton.hpp>
#include <BLIB/ECS/ComponentPool.hpp>
#include <BLIB/Engine/System.hpp>

namespace bl
{
namespace sys
{
class SkeletalAnimationSystem : public engine::System {
public:
    SkeletalAnimationSystem() = default;

    virtual ~SkeletalAnimationSystem() = default;

    /**
     * @brief This is called once on engine startup
     *
     * @param engine The game engine instance
     */
    virtual void init(engine::Engine& engine) override;

    /**
     * @brief This is called each frame with the time to simulate
     *
     * @param stageMutex Reference to a mutex that can be used to synchronize systems in the same
     *                   stage that run in parallel
     *
     * @param dt Time to simulate in seconds
     * @param realDt Time elapsed in real seconds
     * @param residual Simulation time, in seconds, not yet accounted for
     * @param realResidual Real time, in seconds, not yet accounted for
     */
    virtual void update(std::mutex& stageMutex, float dt, float realDt, float residual,
                        float realResidual) override;

private:
    ecs::ComponentPool<com::Skeleton>* skeletons;
};

} // namespace sys
} // namespace bl

#endif
