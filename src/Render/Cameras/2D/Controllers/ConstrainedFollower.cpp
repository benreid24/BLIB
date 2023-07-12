#include <BLIB/Render/Cameras/2D/Controllers/ConstrainedFollower.hpp>

#include <BLIB/Logging.hpp>
#include <BLIB/Render/Cameras/2D/Camera2D.hpp>

namespace bl
{
namespace rc
{
namespace c2d
{
ConstrainedFollower::ConstrainedFollower(ecs::Registry& registry, ecs::Entity follow,
                                         const sf::FloatRect& area, bool fr)
: pool(&registry.getAllComponents<t2d::Transform2D>())
, follow(follow)
, area(area)
, followRotation(fr) {
#ifdef BLIB_DEBUG
    warned = false;
#endif
}

void ConstrainedFollower::update(float) {
    t2d::Transform2D* pos = pool->get(follow);
    if (!pos) {
#ifdef BLIB_DEBUG
        if (!warned) {
            BL_LOG_ERROR << "Cannot follow entity " << follow << " - Transform2D is missing";
            warned = true;
        }
#endif
        return;
    }

#ifdef BLIB_DEBUG
    warned = false;
#endif

    glm::vec2 center = pos->getPosition();
    if (camera().getSize().x >= area.width) { center.x = area.left + area.width * 0.5f; }
    else {
        if (center.x - camera().getSize().x * 0.5f < area.left) {
            center.x = area.left + camera().getSize().x * 0.5f;
        }
        else if (center.x + camera().getSize().x * 0.5f > area.left + area.width) {
            center.x = area.left + area.width - camera().getSize().x * 0.5f;
        }
    }

    if (camera().getSize().y >= area.height) { center.y = area.top + area.height * 0.5f; }
    else {
        if (center.y - camera().getSize().y * 0.5f < area.top) {
            center.y = area.top + camera().getSize().y * 0.5f;
        }
        else if (center.y + camera().getSize().y * 0.5f > area.top + area.height) {
            center.y = area.top + area.height - camera().getSize().y * 0.5f;
        }
    }

    camera().setCenter(center);
    if (followRotation) { camera().setRotation(pos->getRotation()); }
}

} // namespace c2d
} // namespace rc
} // namespace bl
