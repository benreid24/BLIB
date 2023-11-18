#include <BLIB/Graphics/Icon.hpp>

namespace bl
{
namespace gfx
{
Icon::Icon(Type type, const glm::vec2& size)
: IconBase(type, size) {}

void Icon::create(engine::Engine& engine) { SingleShape2D::create(engine); }

} // namespace gfx
} // namespace bl
