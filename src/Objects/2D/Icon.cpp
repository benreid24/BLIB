#include <BLIB/Objects/2D/Icon.hpp>

namespace bl
{
namespace obj
{
namespace o2d
{
Icon::Icon(Type type, const glm::vec2& size)
: IconBase(type, size) {}

void Icon::create(engine::World& world) { SingleShape::create(world); }

} // namespace o2d
} // namespace obj
} // namespace bl
