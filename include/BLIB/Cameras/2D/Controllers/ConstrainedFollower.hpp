#ifndef BLIB_CAMERAS_2D_CONTROLLERS_CONSTRAINEDFOLLOWER_HPP
#define BLIB_CAMERAS_2D_CONTROLLERS_CONSTRAINEDFOLLOWER_HPP

#include <BLIB/Cameras/2D/CameraController2D.hpp>
#include <BLIB/Components/Transform2D.hpp>
#include <BLIB/ECS.hpp>
#include <SFML/Graphics/Rect.hpp>

namespace bl
{
namespace cam
{
/// Instances of 2d camera affectors and controllers
namespace c2d
{
/**
 * @brief 2d camera controller that centers a camera on an entity while keeping the visible region
 *        constrained to a given region. If the camera view is too large it centers on the region
 *
 * @ingroup Cameras
 */
class ConstrainedFollower : public CameraController2D {
public:
    /**
     * @brief Creates a new constrained follower controller
     *
     * @param registry The ECS registry
     * @param follow The entity to follow. Must have a Transform2D component
     * @param area The region to attempt to keep the visible area limited to
     * @param followRotation True to match the rotation of the entity, false to leave unchanged
     */
    ConstrainedFollower(ecs::Registry& registry, ecs::Entity follow, const sf::FloatRect& area,
                        bool followRotation = false);

    /**
     * @brief Destroys the controller
     */
    virtual ~ConstrainedFollower() = default;

    /**
     * @brief Updates the controller to keep the camera centered
     *
     * @param dt Unused
     */
    virtual void update(float dt) override;

private:
    ecs::ComponentPool<com::Transform2D>* pool;
    ecs::Entity follow;
    const sf::FloatRect area;
    const bool followRotation;

#ifdef BLIB_DEBUG
    bool warned;
#endif
};

} // namespace c2d
} // namespace cam
} // namespace bl

#endif
