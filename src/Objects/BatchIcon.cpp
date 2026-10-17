#include <BLIB/Objects/BatchIcon.hpp>

namespace bl
{
namespace obj
{
BatchIcon::BatchIcon(Type type, const glm::vec2& size)
: IconBase(type, size) {}

void BatchIcon::create(engine::Engine& engine, BatchedShapes2D& owner) {
    BatchShape2D::create(engine, owner);
}

} // namespace obj
} // namespace bl
