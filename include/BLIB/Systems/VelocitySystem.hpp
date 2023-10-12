#ifndef BLIB_SYSTEMS_VELOCITY2DSYSTEM_HPP
#define BLIB_SYSTEMS_VELOCITY2DSYSTEM_HPP

#include <BLIB/Components/Transform2D.hpp>
#include <BLIB/Components/Velocity2D.hpp>
#include <BLIB/ECS.hpp>
#include <BLIB/Engine/System.hpp>

namespace bl
{
namespace sys
{
/**
 * @brief System that applies velocity to positions over time
 *
 * @ingroup Systems
 */
class VelocitySystem : public engine::System {
public:
    /**
     * @brief Creates the system
     */
    VelocitySystem();

    /**
     * @brief Destroys the system
     */
    virtual ~VelocitySystem() = default;

private:
    using Tags2D = ecs::Tags<ecs::Require<com::Velocity2D, com::Transform2D>>;

    Tags2D::TView* view2d;

    virtual void update(std::mutex&, float, float, float, float) override;
    virtual void init(engine::Engine& engine) override;
};

} // namespace sys
} // namespace bl

#endif
