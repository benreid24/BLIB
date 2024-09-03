#include <BLIB/Components/Physics2D.hpp>

namespace bl
{
namespace com
{
Physics2D::Physics2D(ecs::Entity owner, Transform2D& transform, b2BodyId bodyId)
: entity(owner)
, transform(&transform)
, bodyId(bodyId) {
    b2Body_SetUserData(bodyId, this);
}

} // namespace com
} // namespace bl
