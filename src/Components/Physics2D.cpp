#include <BLIB/Components/Physics2D.hpp>

namespace bl
{
namespace com
{
Physics2D::Physics2D(Transform2D& transform, b2BodyId bodyId)
: transform(&transform)
, bodyId(bodyId) {}

} // namespace com
} // namespace bl
