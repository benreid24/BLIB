#include <BLIB/Cameras/2D/Controllers/ConstrainedFollower.hpp>

#include <BLIB/Cameras/2D/Camera2D.hpp>
#include <BLIB/Logging.hpp>

namespace bl
{
namespace cam
{
namespace c2d
{
ConstrainedFollower::ConstrainedFollower(ecs::Registry& registry, ecs::Entity follow,
                                         const sf::FloatRect& area, bool fr)
: pool(&registry.getAllComponents<com::Transform2D>())
, follow(follow)
, area(area)
, followRotation(fr) {
#ifdef BLIB_DEBUG
    warned = false;
#endif
}

void ConstrainedFollower::update(float) {
    com::Transform2D* pos = pool->get(follow);
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

    glm::vec2 center = pos->getGlobalPosition();
    if (camera().getSize().x >= area.size.x) {
        //
        center.x = area.position.x + area.size.x * 0.5f;
    }
    else {
        if (center.x - camera().getSize().x * 0.5f < area.position.x) {
            center.x = area.position.x + camera().getSize().x * 0.5f;
        }
        else if (center.x + camera().getSize().x * 0.5f > area.position.x + area.size.x) {
            center.x = area.position.x + area.size.x - camera().getSize().x * 0.5f;
        }
    }

    if (camera().getSize().y >= area.size.y) { center.y = area.position.y + area.size.y * 0.5f; }
    else {
        if (center.y - camera().getSize().y * 0.5f < area.position.y) {
            center.y = area.position.y + camera().getSize().y * 0.5f;
        }
        else if (center.y + camera().getSize().y * 0.5f > area.position.y + area.size.y) {
            center.y = area.position.y + area.size.y - camera().getSize().y * 0.5f;
        }
    }

    camera().setCenter(center);
    if (followRotation) { camera().setRotation(pos->getRotation()); }
}

} // namespace c2d
} // namespace cam
} // namespace bl
