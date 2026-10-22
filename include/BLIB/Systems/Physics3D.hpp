#ifndef BLIB_SYSTEMS_PHYSICS3D_HPP
#define BLIB_SYSTEMS_PHYSICS3D_HPP

#include <Jolt/Jolt.h>

#include <BLIB/Components/Physics3D.hpp>
#include <BLIB/Engine/System.hpp>
#include <Jolt/Core/TempAllocator.h>

namespace bl
{
namespace sys
{
/**
 * @brief Engine system for 3d physics
 */
class Physics3D : public engine::System {
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
    JPH::TempAllocatorImpl joltFrameAllocator;

    virtual void init(engine::Engine& engine) override;
    virtual void update(std::mutex& stageMutex, float dt, float realDt, float residual,
                        float realResidual) override;
};

} // namespace sys
} // namespace bl

#endif
