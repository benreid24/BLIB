#include <BLIB/Objects/Icon.hpp>

namespace bl
{
namespace obj
{
Icon::Icon(Type type, const glm::vec2& size)
: IconBase(type, size) {}

void Icon::create(engine::World& world) { SingleShape2D::create(world); }

} // namespace obj
} // namespace bl
