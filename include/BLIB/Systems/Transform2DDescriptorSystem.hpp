#ifndef BLIB_SYSTEMS_TRANSFORM2DDESCRIPTORSYSTEM_HPP
#define BLIB_SYSTEMS_TRANSFORM2DDESCRIPTORSYSTEM_HPP

#include <BLIB/Components/Transform2D.hpp>
#include <BLIB/Components/Velocity2D.hpp>
#include <BLIB/ECS.hpp>
#include <BLIB/Engine/System.hpp>

namespace bl
{
namespace sys
{
/**
 * @brief System that updates 2d transforms into renderer descriptor sets. Also performs
 *        interpolation for entities that also have velocity
 *
 * @ingroup Systems
 */
class Transform2DDescriptorSystem : public engine::System {
public:
    /**
     * @brief Initializes the system
     */
    Transform2DDescriptorSystem();

    /**
     * @brief Destroys the system
     */
    virtual ~Transform2DDescriptorSystem() = default;

private:
    using InterpTags = ecs::Tags<ecs::Require<com::Velocity2D, com::Transform2D>>;
    using PosTags =
        ecs::Tags<ecs::Require<com::Transform2D>, ecs::Optional<>, ecs::Exclude<com::Velocity2D>>;

    InterpTags::TView* interpView;
    PosTags::TView* posOnlyView;

    virtual void update(std::mutex&, float, float, float, float) override;
    virtual void init(engine::Engine& engine) override;
};

} // namespace sys
} // namespace bl

#endif
