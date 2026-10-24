#ifndef BLIB_SYSTEMS_PHYSICS3D_HPP
#define BLIB_SYSTEMS_PHYSICS3D_HPP

#include <Jolt/Jolt.h>

#include <BLIB/Components/Physics3D.hpp>
#include <BLIB/Engine/Events/Worlds.hpp>
#include <BLIB/Engine/System.hpp>
#include <BLIB/Signals/Listener.hpp>
#include <BLIB/Systems/Jolt/JobSystem.hpp>
#include <Jolt/Core/TempAllocator.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <optional>
#include <vector>

namespace bl
{
namespace sys
{
/**
 * @brief Engine system for 3d physics
 */
class Physics3D
: public engine::System
, public sig::Listener<engine::event::WorldCreated, engine::event::WorldDestroyed> {
public:
    /**
     * @brief Creates the system
     */
    Physics3D();

    /**
     * @brief Destroys the system
     */
    virtual ~Physics3D() = default;

private:
    std::optional<jolt::JobSystem> jobSystem;
    JPH::TempAllocatorImpl joltFrameAllocator;
    std::vector<JPH::PhysicsSystem*> activePhysicsSystems;

    virtual void init(engine::Engine& engine) override;
    virtual void earlyCleanup() override;
    virtual void update(std::mutex& stageMutex, float dt, float realDt, float residual,
                        float realResidual) override;

    virtual void process(const engine::event::WorldCreated& worldCreate) override;
    virtual void process(const engine::event::WorldDestroyed& worldDestroy) override;
};

} // namespace sys
} // namespace bl

#endif
