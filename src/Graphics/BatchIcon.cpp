#include <BLIB/Graphics/BatchIcon.hpp>

namespace bl
{
namespace gfx
{
BatchIcon::BatchIcon(Type type, const glm::vec2& size)
: IconBase(type, size) {}

void BatchIcon::create(engine::Engine& engine, BatchedShapes2D& owner) {
    BatchShape2D::create(engine, owner);
}

} // namespace gfx
} // namespace bl
