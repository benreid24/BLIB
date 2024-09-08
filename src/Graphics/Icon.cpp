#include <BLIB/Graphics/Icon.hpp>

namespace bl
{
namespace gfx
{
Icon::Icon(Type type, const glm::vec2& size)
: IconBase(type, size) {}

void Icon::create(engine::World& world) { SingleShape2D::create(world); }

} // namespace gfx
} // namespace bl
